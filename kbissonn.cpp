#include "kbissonn.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <filesystem>
#include <regex>
#include <set>
#include <sys/types.h>
#include <sys/stat.h>
#include <functional>

using namespace std;


// Checks for cron jobs in /etc/cron.d, /etc/cron.daily, /etc/cron.hourly, /etc/cron.monthly, /etc/cron.weekly
void check_cron_jobs() {
    // First check for user permissions (requires root permission)
    if (geteuid() != 0) {
        cerr << "You need \"root\" permission to check cron jobs. (Run program as sudo)" << endl;
        return;
    }
    // Directories to check for cron jobs
    vector<string> cron_dirs = {
        "/etc/cron.d",
        "/etc/cron.daily",
        "/etc/cron.hourly",
        "/etc/cron.monthly",
        "/etc/cron.weekly",
        "/var/spool/cron",
        "/var/spool/cron/crontabs"
    };

    set<string> checked_files;
    // Vectors to record checked paths (in discovery order) and writable cron-called ones
    vector<string> all_checked_paths;
    vector<string> writable_paths;

    // Process /etc/crontab if present
    filesystem::path etccrontab = "/etc/crontab";
    if (filesystem::exists(etccrontab)) {
        process_file(etccrontab, checked_files, true, &all_checked_paths, &writable_paths);
    }

    // Go through and process each file from the cron directories
    for (size_t di = 0; di < cron_dirs.size(); ++di) {
        const string &dir = cron_dirs[di];
        try {
            if (!filesystem::exists(dir)) {
                continue;
            }
            for (auto it = filesystem::directory_iterator(dir); it != filesystem::directory_iterator(); ++it) {
                const filesystem::directory_entry &entry = *it;
                process_file(entry.path(), checked_files, true, &all_checked_paths, &writable_paths);
            }
        } catch (const filesystem::filesystem_error &e) {
            cerr << "Error reading " << dir << ": " << e.what() << endl;
        }
    }

    cout << "Cron job scan complete. Checked " << checked_files.size() << " distinct path(s)." << endl;
    cout << "Writable paths referenced by cron jobs (world-writable):\n";
    for (size_t i = 0; i < writable_paths.size(); ++i) {
        cout << "  " << writable_paths[i] << '\n';
    }
}

// Helper functions
// Go through and see if it is a schedule token (5 * * * *, @reboot, etc)
bool is_schedule_token(const string &t) {
    static regex r("^[0-9\\*\\/,\\-]+$");
    if (t.empty()) {
        return false;
    }
    if (t.front() == '@') {
        return true;
    }
    return regex_match(t, r);
}

// Split a string into tokens, preserving quoted substrings
vector<string> split_tokens(const string &s) {
    vector<string> out;
    string cur;
    bool in_squote = false;
    bool in_dquote = false;
    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];
        if (c == '\'' && !in_dquote) {
            in_squote = !in_squote;
            cur.push_back(c);
            continue;
        }
        if (c == '"' && !in_squote) {
            in_dquote = !in_dquote;
            cur.push_back(c);
            continue;
        }
        if (isspace((unsigned char)c) && !in_squote && !in_dquote) {
            if (!cur.empty()) { out.push_back(cur); cur.clear(); }
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

// Removing extra quotes and special chars
string sanitize_token(string t) {
    if (t.size() >= 2 && ((t.front() == '"' && t.back() == '"') || (t.front() == '\'' && t.back() == '\''))) {
        t = t.substr(1, t.size() - 2);
    }
    while (!t.empty() && (t.back()=='|' || t.back()=='&' || t.back()=='>' || t.back()=='<' || t.back()==';' || t.back()==')')){
        t.pop_back();
    }
    while (!t.empty() && (t.front()=='(')){
        t.erase(t.begin());
    }
    return t;
}

// Get everthring that looks like a path from a command string
vector<string> extract_paths(const string &cmd) {
    vector<string> found;
    vector<string> tokens = split_tokens(cmd);
    for (size_t ti = 0; ti < tokens.size(); ++ti) {
        const string &tk = tokens[ti];
        string s = sanitize_token(tk);
        if (s.empty()) {
            continue;
        }
        if (s.find('=') != string::npos && s.find('/') == string::npos) {
            continue;
        }
        if (s.find('/') != string::npos) {
            size_t pos = s.find('>');
            if (pos != string::npos && pos == 0) {
                continue;
            }
            found.push_back(s);
        }
    }
    return found;
}

// Look line by line through a file and get commands/paths, currently prints
// files to the console with their writability status
void process_file(const filesystem::path &p, set<string> &checked_files, bool called_from_cron, std::vector<std::string> *all_checked_paths, std::vector<std::string> *writable_paths) {
    if (!filesystem::is_regular_file(p)) {
        return;
    }
    ifstream f(p);
    if (!f) {
        return;
    }
    string line;
    int linenumber = 0;
    while (getline(f, line)) {
        ++linenumber;
        size_t start = line.find_first_not_of(" \t");
        if (start == string::npos) {
            continue;
        }
        string sline = line.substr(start);
        if (sline.empty() || sline[0] == '#') {
            continue;
        }
        size_t eq = sline.find('=');
        if (eq != string::npos && sline.find(' ') == string::npos) {
            continue;
        }

        vector<string> tokens = split_tokens(sline);
        size_t idx = 0;
    while (idx < tokens.size() && is_schedule_token(tokens[idx])) ++idx;
        if (idx < tokens.size() && tokens.size() > 1) {
            string tk = tokens[idx];
            if (tk.find('/') == string::npos && tk.find('=') == string::npos && !is_schedule_token(tk)) {
                idx++;
            }
        }
        string cmd;
        for (size_t i = idx; i < tokens.size(); ++i) {
            if (!cmd.empty()) cmd.push_back(' ');
            cmd += tokens[i];
        }
        if (cmd.empty()) {
            continue;
        }
        vector<string> paths = extract_paths(cmd);
        for (size_t pi = 0; pi < paths.size(); ++pi) {
            const string &pth = paths[pi];
            string candidate = pth;
            try {
                if (!candidate.empty() && candidate[0] == '/') {
                    filesystem::path fp = candidate;
                    string resolved = fp.lexically_normal().string();
                    candidate = resolved;
                }
            } catch (...) {}

            if (checked_files.count(candidate)) {
                continue;
            }
            checked_files.insert(candidate);
            if (all_checked_paths) {
                all_checked_paths->push_back(candidate);
            }

            // When called from cron, avoid noisy per-line printing; instead
            // collect paths in the provided vectors and let the caller decide
            // what to print. For non-cron callers, keep original behavior.
            if (!called_from_cron) {
                cout << p << ":" << linenumber << " -> referencing: " << candidate;
                if (filesystem::exists(candidate)) {
                    cout << " (exists) ";
                    // Determine if world-writable. Use stat to check others write bit.
                    bool world_writable = false;
                    struct stat st;
                    if (stat(candidate.c_str(), &st) == 0) {
                        world_writable = (st.st_mode & S_IWOTH) != 0;
                    }
                    // Only report writable when called from cron and file is world-writable.
                    if (called_from_cron && world_writable) {
                        cout << "[WRITABLE]";
                        if (writable_paths) writable_paths->push_back(candidate);
                    } else {
                        cout << "[not writable]";
                    }
                    cout << endl;
                } else {
                    cout << " (missing)" << endl;
                }
            } else {
                // Cron caller: collect writable paths if world-writable
                if (filesystem::exists(candidate)) {
                    struct stat st;
                    if (stat(candidate.c_str(), &st) == 0) {
                        if ((st.st_mode & S_IWOTH) != 0) {
                            if (writable_paths) writable_paths->push_back(candidate);
                        }
                    }
                }
            }
        }
    }
}

// Verbose option to list all called files
void list_cronjob_commands() {
    vector<string> cron_dirs = {
        "/etc/cron.d",
        "/etc/cron.daily",
        "/etc/cron.hourly",
        "/etc/cron.monthly",
        "/etc/cron.weekly"
    };

    for (const string &dir : cron_dirs) {
        cout << "Checking directory: " << dir << endl;
        for (const filesystem::directory_entry &entry : filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                ifstream cronfile(entry.path());
                string line;
                cout << "File: " << entry.path() << endl;
                while (getline(cronfile, line)) {
                    // Skip comments and empty lines
                    if (line.empty() || line[0] == '#') {
                        continue;
                    }
                    stringstream ss(line);
                    string field, command;
                    int field_count = 0;
                    // Skip schedule fields (usually 5 for crontab, sometimes 6+ for /etc/cron.d)
                    while (ss >> field && field_count < 5) {
                        ++field_count;
                    }
                    // The rest is the command
                    getline(ss, command);
                    if (!command.empty()) {
                        cout << "  Command: " << command << endl;
                    }
                }
            }
        }
    }
}


//Sudoers file check
void check_sudoers() {
    // First check for user permissions (requires root permission)
    if (geteuid() != 0) {
        cerr << "You need \"root\" permission to check sudoers. (Run program as sudo)" << endl;
        return;
    }
    vector<filesystem::path> sudoers_files = {
        "/etc/sudoers"
    };
    // get contents of /etc/sudoers.d/
    filesystem::path sudoers_d = "/etc/sudoers.d";
    if (filesystem::exists(sudoers_d) && filesystem::is_directory(sudoers_d)) {
        for (auto it = filesystem::directory_iterator(sudoers_d); it != filesystem::directory_iterator(); ++it) {
            const filesystem::directory_entry &entry = *it;
            if (entry.is_regular_file()){
                sudoers_files.push_back(entry.path());
            }
        }
    }

    // Look at each sudoers file and add users to a vector 
    // (looks like <user[,user2]> <host> = <rules>)
    // and ignore commented out lines
    set<string> users;
    regex rule_re(R"(^\s*([^\s]+(?:\s*,\s*[^\s]+)*)\s+([^\s]+)\s*=\s*(.*))");
    // process each collected sudoers file
    for (size_t i = 0; i < sudoers_files.size(); ++i) {
        const filesystem::path &p = sudoers_files[i];
        try {
            if (!filesystem::exists(p) || !filesystem::is_regular_file(p)){
                continue;
            }
            ifstream f(p);
            if (!f){ 
                continue;
            }
            string line;
            int lineno = 0;
            while (getline(f, line)) {
                ++lineno;
                size_t start = line.find_first_not_of(" \t");
                if (start == string::npos){ 
                    continue;
                }
                string sline = line.substr(start);
                if (sline.empty() || sline[0] == '#'){ 
                    continue;
                }

                // Skip common non-rule lines
                const vector<string> skip_prefixes = {"Defaults", "User_Alias", "Runas_Alias", "Cmnd_Alias", "Host_Alias", "Include", "#include"};
                bool skip = false;
                for (size_t spi = 0; spi < skip_prefixes.size(); ++spi) {
                    const string &pref = skip_prefixes[spi];
                    if (sline.rfind(pref, 0) == 0) { 
                        skip = true; 
                        break; 
                    }
                }
                if (skip){
                    continue;
                }

                smatch m;
                if (regex_match(sline, m, rule_re)) {
                    if (m.size() >= 2) {
                        string userlist = m[1];
                        // split on commas
                        vector<string> parts;
                        string tmp = userlist;
                        size_t pos = 0;
                        while ((pos = tmp.find(',')) != string::npos) {
                            parts.push_back(tmp.substr(0, pos));
                            tmp.erase(0, pos + 1);
                        }
                        if (!tmp.empty()){
                            parts.push_back(tmp);
                        }

                        for (size_t pi = 0; pi < parts.size(); ++pi) {
                            const string &part = parts[pi];
                            // trim
                            size_t a = part.find_first_not_of(" \t");
                            if (a == string::npos){
                                continue;
                            }
                            size_t b = part.find_last_not_of(" \t");
                            string u = part.substr(a, b - a + 1);
                            if (u.empty()){
                                continue;
                            }
                            if (u.front() == '%'){
                                continue; // skip groups
                            }
                            users.insert(u);
                        }
                    }
                }
            }
        } catch (const filesystem::filesystem_error &e) {
            cerr << "Error reading sudoers file " << p << ": " << e.what() << endl;
        }
    }

    // Print results to console
    cout << "Sudoers scan complete. Found " << users.size() << " unique user(s) in sudoers files." << endl;
    for (auto uit = users.begin(); uit != users.end(); ++uit){
        cout << "  " << *uit << endl;
    }
}