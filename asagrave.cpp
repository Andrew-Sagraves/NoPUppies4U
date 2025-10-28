#include "asagrave.h"
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <array>
#include <memory>
using namespace std;

extern bool VERBOSE;

// Retrieve PATH entries as a vector of strings
std::vector<std::string> get_paths() {
  std::vector<std::string> paths;
  const char *path_env = getenv("PATH");
  if (!path_env)
    return paths;

  std::string path_str(path_env);
  std::stringstream ss(path_str);
  std::string dir;
  while (std::getline(ss, dir, ':')) {
    paths.push_back(dir); // keep empty entries (treated as ".")
  }
  return paths;
}

// Check if a directory (and its parents) are world-writable
bool pathutil_is_world_writable_dir(const std::string &dir) {
  if (dir.empty())
    return false;

  std::string path = dir;

  // Remove trailing slashes
  while (path.length() > 1 && path.back() == '/') {
    path.pop_back();
  }

  struct ::stat st;

  while (!path.empty()) {
    if (stat(path.c_str(), &st) == -1)
      return false;
    if (S_ISDIR(st.st_mode) && (st.st_mode & S_IWOTH))
      return true;

    if (path == "/")
      break; // root reached

    auto pos = path.find_last_of('/');
    if (pos == std::string::npos)
      break;

    if (pos == 0)
      path = "/";
    else
      path = path.substr(0, pos);
  }

  return false;
}

// Check if directory string is relative
bool pathutil_is_relative_dir(const std::string &dir) {
  if (dir.empty() || dir == ".")
    return true;
  return dir[0] != '/';
}

// Check if a directory or its files have vulnerabilities
PathVulnResult pathutil_dir_has_vulnerability(const std::string &dir) {
  std::string effective = dir.empty() ? "." : dir;

  if (pathutil_is_relative_dir(effective)) {
    return {true, "Relative directory"};
  }

  if (pathutil_is_world_writable_dir(effective)) {
    return {true, "World-writable directory (or parent)"};
  }

  // Scan files inside the directory for world-writable files
  DIR *dp = opendir(effective.c_str());
  if (!dp)
    return {false, ""};

  struct dirent *entry;
  struct ::stat st;
  std::string filepath;

  while ((entry = readdir(dp)) != nullptr) {
    filepath = effective + "/" + entry->d_name;
    if (stat(filepath.c_str(), &st) == -1)
      continue;
    if (S_ISREG(st.st_mode) && (st.st_mode & S_IWOTH)) {
      closedir(dp);
      return {true, "World-writable file: " + filepath};
    }
  }

  closedir(dp);
  return {false, ""};
}

// Scan PATH entries and log vulnerabilities, including duplicate executables
int get_path_vulnerabilities(const std::vector<std::string> &paths) {
  std::ofstream report("PATH.txt");
  int problems = 0;

  // Map to track executable names -> first directory seen
  std::unordered_map<std::string, std::string> exe_map;

  for (const auto &dir : paths) {
    PathVulnResult result = pathutil_dir_has_vulnerability(dir);
    if (result.vulnerable) {
      report << "[!] " << result.reason << " (" << (dir.empty() ? "." : dir)
             << ")\n";
      problems++;
    }

    // Scan executables for duplicates
    std::string effective = dir.empty() ? "." : dir;
    DIR *dp = opendir(effective.c_str());
    if (!dp)
      continue;

    struct dirent *entry;
    struct ::stat st;
    std::string filepath;

    while ((entry = readdir(dp)) != nullptr) {
      filepath = effective + "/" + entry->d_name;
      if (stat(filepath.c_str(), &st) == -1)
        continue;

      if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR)) {
        std::string exe_name = entry->d_name;
        if (exe_map.find(exe_name) != exe_map.end()) {
          report << "[!] Duplicate executable: " << exe_name << " found in "
                 << effective << " (also in " << exe_map[exe_name] << ")\n";
          problems++;
        } else {
          exe_map[exe_name] = effective;
        }
      }
    }

    closedir(dp);
  }

  report.close();
  return problems;
}
// --------------------------------------------------------------
// Utility functions
// --------------------------------------------------------------

// Create a directory if it doesnÃÂ¢ÃÂÃÂt exist
void ensure_dir(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        mkdir(path.c_str(), 0755);
    }
}

// Append a line to a log file
void write_log(const std::string& file, const std::string& message) {
    std::ofstream out(file.c_str(), std::ios::app);
    if (out.is_open()) {
        out << message << "\n";
    }
}

// Simple recursive directory traversal using POSIX API
void list_files_recursive(const std::string& base, std::vector<std::string>& files) {
    DIR* dir = opendir(base.c_str());
    if (!dir) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        std::string fullpath = base + "/" + entry->d_name;
        struct stat st;
        if (lstat(fullpath.c_str(), &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                list_files_recursive(fullpath, files);
            } else if (S_ISREG(st.st_mode)) {
                files.push_back(fullpath);
            }
        }
    }
    closedir(dir);
}

// --------------------------------------------------------------
// 1. Passwordless sudo access check
// --------------------------------------------------------------
bool passwordless_sudo_access(const std::string& logDir) {

    if (VERBOSE) {
        cout << "[SUDO] Checking for passwordless sudo access..." << endl;
    }

    string logFile = logDir;
    write_log(logFile, "=== Checking for passwordless SUDO access ===");

    bool found = false;
    const char* username = getenv("USER");
    if (!username) username = "UNKNOWN";

    vector<string> sudoFiles;
    sudoFiles.push_back("/etc/sudoers");

    // Include any files under /etc/sudoers.d
    DIR* d = opendir("/etc/sudoers.d");
    if (d) {
        struct dirent* e;
        while ((e = readdir(d)) != NULL) {
            if (e->d_name[0] == '.') continue;
            sudoFiles.push_back(string("/etc/sudoers.d/") + e->d_name);
        }
        closedir(d);
    }

    for (size_t i = 0; i < sudoFiles.size(); ++i) {

        if (VERBOSE) {
            cout << "[SUDO] Scanning: " << sudoFiles[i] << endl;
        }

        ifstream in(sudoFiles[i].c_str());
        if (!in.is_open()) {
            if (VERBOSE) {
                cout << "[SUDO] Could not open: " << sudoFiles[i] << endl;
            }
            continue;
        }

        string line;
        while (getline(in, line)) {
            if (line.find("NOPASSWD") != string::npos &&
                (line.find(username) != string::npos || line.find("ALL") != string::npos)) {
                
                found = true;

                if (VERBOSE) {
                    cout << "[SUDO] Passwordless entry found: " << line << endl;
                }

                write_log(logFile, "Passwordless sudo entry found in: " + sudoFiles[i]);
                write_log(logFile, "  -> " + line);
            }
        }
    }

    if (!found) {
        write_log(logFile, "No passwordless sudo entries found.");
    }

    if (VERBOSE) {
        cout << "[SUDO] Sudo audit complete." << endl;
    }

    return found;
}

// --------------------------------------------------------------
// 2. World-writable SSH keys
// --------------------------------------------------------------
bool world_writable_ssh_keys(const std::string& logDir) {

    if (VERBOSE) {
        cout << "[SSH] Checking SSH key permissions..." << endl;
    }

    std::string logFile = logDir;
    write_log(logFile, "=== Checking for world-writable SSH keys ===");

    bool found = false;
    std::vector<std::string> sshDirs;

    const char* home = getenv("HOME");
    if (home) {
        sshDirs.push_back(std::string(home) + "/.ssh");
    }
    sshDirs.push_back("/etc/ssh");

    for (size_t i = 0; i < sshDirs.size(); ++i) {

        if (VERBOSE) {
            cout << "[SSH] Scanning directory: " << sshDirs[i] << endl;
        }

        std::vector<std::string> files;
        list_files_recursive(sshDirs[i], files);
        for (size_t j = 0; j < files.size(); ++j) {
            struct stat st;
            if (stat(files[j].c_str(), &st) == 0) {
                if (st.st_mode & S_IWOTH) {
                    found = true;

                    if (VERBOSE) {
                        cout << "[SSH] World-writable SSH file: " << files[j] << endl;
                    }

                    write_log(logFile, "World-writable SSH file: " + files[j]);
                }
            }
        }
    }

    if (!found) {
        write_log(logFile, "No world-writable SSH keys found.");
    }

    if (VERBOSE) {
        cout << "[SSH] SSH permissions audit complete." << endl;
    }

    return found;
}


// --------------------------------------------------------------
// 3. SUID binary audit
// --------------------------------------------------------------
bool suid_binary_audit(const std::string& logDir) {

    if (VERBOSE) {
        cout << "[SUID] Starting SUID scan..." << endl;
    }

    std::string logFile = logDir;
    write_log(logFile, "=== Scanning for SUID binaries ===");

    bool found = false;
    int count = 0;

    std::vector<std::string> dirs;
    dirs.push_back("/bin");
    dirs.push_back("/sbin");
    dirs.push_back("/usr/bin");
    dirs.push_back("/usr/sbin");
    dirs.push_back("/usr/local/bin");
    dirs.push_back("/usr/local/sbin");
    dirs.push_back("/opt");

    for (size_t i = 0; i < dirs.size(); ++i) {

        if (VERBOSE) {
            cout << "[SUID] Scanning directory: " << dirs[i] << endl;
        }

        std::vector<std::string> files;
        list_files_recursive(dirs[i], files);
        for (size_t j = 0; j < files.size(); ++j) {
            struct stat st;
            if (lstat(files[j].c_str(), &st) == 0) {
                if (st.st_mode & S_ISUID) {
                    found = true;
                    count++;

                    if (VERBOSE) {
                        cout << "[SUID] SUID binary found: " << files[j] << endl;
                    }

                    write_log(logFile, "SUID binary: " + files[j]);
                }
            }
        }
    }

    write_log(logFile, "Total SUID binaries found: " + std::to_string(count));

    if (VERBOSE) {
        cout << "[SUID] Scan complete. Total found: " << count << endl;
    }

    return found;
}

static std::string exec_cmd(const std::string &cmd) {
    std::array<char, 256> buf{};
    std::string out;
    std::unique_ptr<FILE, int(*)(FILE*)> p(popen(cmd.c_str(), "r"), pclose);
    if (!p) return "";
    while (fgets(buf.data(), buf.size(), p.get())) out += buf.data();
    while (!out.empty() && (out.back() == '\n' || out.back() == '\r')) out.pop_back();
    return out;
}

static bool stat_path(const std::string &p, uid_t &u, gid_t &g, mode_t &m) {
    struct stat st; if (stat(p.c_str(), &st)) return false;
    u = st.st_uid; g = st.st_gid; m = st.st_mode; return true;
}

static bool is_writable_by_nonroot(const std::string &p) {
    uid_t u; gid_t g; mode_t m; if (!stat_path(p,u,g,m)) return false;
    return ((m&S_IWUSR && u) || (m&S_IWGRP && g) || (m&S_IWOTH));
}

static std::string md5(const std::string &p) {
    return exec_cmd("md5sum '" + p + "' 2>/dev/null | awk '{print $1}'");
}

static void log_line(const std::string &f, const std::string &l) {
    std::ofstream o(f,std::ios::app); if(o) o<<l<<'\n';
}

/* -------- SUID / Package ownership audit -------- */
bool suid_package_audit(const std::string &log) {
    bool issues=false;
    log_line(log,"=== SUID/SGID package audit ===");
    std::istringstream s(exec_cmd("find / -xdev -type f \\( -perm -4000 -o -perm -2000 \\) -print 2>/dev/null"));
    for(std::string f; std::getline(s,f);){
        uid_t u; gid_t g; mode_t m;
        if(!stat_path(f,u,g,m)) continue;
        std::string pkg,sys=""; 
        std::string d=exec_cmd("dpkg -S '"+f+"' 2>/dev/null|head -1");
        if(d.find(':')!=std::string::npos){pkg=d.substr(0,d.find(':'));sys="dpkg";}
        else {
            std::string r=exec_cmd("rpm -qf '"+f+"' 2>/dev/null");
            if(r.find("not owned")==std::string::npos){pkg=r;sys="rpm";}
        }
        std::ostringstream ev;
        ev<<"File:"<<f<<" perms:"<<std::oct<<(m&07777)<<std::dec;
        if(pkg.empty()){
            log_line(log,ev.str()+" | pkg:(none) => suspicious"); issues=true; continue;
        }
        bool bad=false;
        if(sys=="dpkg"){
            std::string info="/var/lib/dpkg/info/"+pkg+".md5sums";
            std::string ref=exec_cmd("grep -F ' "+f+"' '"+info+"' 2>/dev/null|awk '{print $1}'");
            std::string local=md5(f);
            if(!ref.empty()&&!local.empty()&&ref!=local) bad=true;
        } else if(sys=="rpm"){
            bad=!exec_cmd("rpm -Vf '"+f+"' 2>/dev/null").empty();
        }
        log_line(log,ev.str()+" pkg:"+pkg+(bad?" => modified":" => ok"));
        if(bad) issues=true;
    }
    log_line(log,"=== End SUID/SGID audit ===");
    return issues;
}

/* -------- systemd unit audit -------- */
bool systemd_unit_audit(const std::string &log) {
    bool issues=false;
    log_line(log,"=== systemd unit audit ===");
    std::vector<std::string> dirs={"/etc/systemd/system","/lib/systemd/system","/usr/lib/systemd/system","/run/systemd/system"};
    std::vector<std::string> units;
    for(auto &d:dirs){
        std::istringstream s(exec_cmd("find '"+d+"' -type f \\( -name '*.service' -o -name '*.timer' \\) -print 2>/dev/null"));
        for(std::string u;std::getline(s,u);) if(!u.empty()) units.push_back(u);
    }
    for(auto &u:units){
        uid_t uid; gid_t gid; mode_t mode;
        if(stat_path(u,uid,gid,mode)&&uid){
            log_line(log,"non-root owned unit:"+u); issues=true;
        }
        auto pos=u.find_last_of('/'); std::string dir=u.substr(0,pos);
        if(is_writable_by_nonroot(dir)){log_line(log,"unit dir writable:"+dir); issues=true;}
        std::ifstream f(u); if(!f) continue;
        std::string line;
        while(std::getline(f,line)){
            std::string t=line; t.erase(0,t.find_first_not_of(" \t"));
            if(t.rfind("ExecStart",0)&&t.rfind("ExecStartPre",0)&&t.rfind("ExecStartPost",0)) continue;
            auto eq=t.find('='); if(eq==std::string::npos) continue;
            std::string cmd=t.substr(eq+1);
            std::istringstream ss(cmd); std::string exe; ss>>exe;
            if(exe.empty()||exe[0]!='/') continue;
            std::string parent=exe.substr(0,exe.find_last_of('/'));
            if(is_writable_by_nonroot(parent)||is_writable_by_nonroot(exe)||exe.find("/tmp/")==0){
                log_line(log,"risk Exec:"+exe+" in "+u); issues=true;
            }
        }
    }
    log_line(log,"=== End systemd audit ===");
    return issues;
}

