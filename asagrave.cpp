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

// Global verbose flag
extern bool VERBOSE;
// Forward declarations (if asagrave.h doesn't declare them)
std::vector<std::string> get_paths();
bool passwordless_sudo_access(const std::string& logDir);
bool world_writable_ssh_keys(const std::string& logDir);
bool suid_binary_audit(const std::string& logDir);
bool suid_package_audit(const std::string &log);
bool systemd_unit_audit(const std::string &log);
int get_path_vulnerabilities(const std::vector<std::string> &paths);

// --------------------------------------------------------------
// Utility: write_log - write a message to file and mirror to stdout when VERBOSE
// --------------------------------------------------------------
void write_log(const std::string& file, const std::string& message) {
    std::ofstream out(file.c_str(), std::ios::app);
    if (out.is_open()) {
        out << message << "\n";
    }
    if (VERBOSE) {
        cout << message << endl;
    }
}

// A convenience that prefixes debug text and writes to both console & log.
// Use when you want to ensure a debug line is both printed and saved.
void verbose_log(const std::string &logfile, const std::string &prefix, const std::string &message) {
    std::string line = prefix.empty() ? message : (prefix + " " + message);
    write_log(logfile, line);
}

// --------------------------------------------------------------
// Retrieve PATH entries as a vector of strings
// --------------------------------------------------------------
std::vector<std::string> get_paths() {
  std::vector<std::string> paths;
  const char *path_env = getenv("PATH");
  if (!path_env) {
    if (VERBOSE) cout << "[PATH] PATH environment variable is empty." << endl;
    return paths;
  }

  std::string path_str(path_env);
  std::stringstream ss(path_str);
  std::string dir;
  while (std::getline(ss, dir, ':')) {
    paths.push_back(dir); // keep empty entries (treated as ".")
  }

  if (VERBOSE) {
    cout << "[PATH] Retrieved " << paths.size() << " PATH entries." << endl;
  }

  return paths;
}

// --------------------------------------------------------------
// Path utilities
// --------------------------------------------------------------
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
    if (stat(path.c_str(), &st) == -1) {
      if (VERBOSE) cout << "[PATHUTIL] stat failed for: " << path << endl;
      return false;
    }
    if (S_ISDIR(st.st_mode) && (st.st_mode & S_IWOTH)) {
      if (VERBOSE) cout << "[PATHUTIL] World-writable directory detected: " << path << endl;
      return true;
    }

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

bool pathutil_is_relative_dir(const std::string &dir) {
  if (dir.empty() || dir == ".")
    return true;
  return dir[0] != '/';
}

PathVulnResult pathutil_dir_has_vulnerability(const std::string &dir) {
  std::string effective = dir.empty() ? "." : dir;

  if (pathutil_is_relative_dir(effective)) {
    if (VERBOSE) {
      cout << "[PATHUTIL] Relative directory considered vulnerable: " << effective << endl;
      write_log("PATH.txt", "[DEBUG] Relative directory: " + effective);
    }
    return {true, "Relative directory"};
  }

  if (pathutil_is_world_writable_dir(effective)) {
    if (VERBOSE) {
      cout << "[PATHUTIL] World-writable directory (or parent) for: " << effective << endl;
      write_log("PATH.txt", "[DEBUG] World-writable directory (or parent): " + effective);
    }
    return {true, "World-writable directory (or parent)"};
  }

  // Scan files inside the directory for world-writable files
  DIR *dp = opendir(effective.c_str());
  if (!dp) {
    if (VERBOSE) {
      cout << "[PATHUTIL] Could not open directory (skipping): " << effective << endl;
      write_log("PATH.txt", "[DEBUG] Could not open dir: " + effective);
    }
    return {false, ""};
  }

  struct dirent *entry;
  struct ::stat st;
  std::string filepath;

  while ((entry = readdir(dp)) != nullptr) {
    filepath = effective + "/" + entry->d_name;
    if (stat(filepath.c_str(), &st) == -1)
      continue;
    if (S_ISREG(st.st_mode) && (st.st_mode & S_IWOTH)) {
      closedir(dp);
      if (VERBOSE) {
        cout << "[PATHUTIL] Found world-writable file: " << filepath << endl;
        write_log("PATH.txt", "[DEBUG] World-writable file: " + filepath);
      }
      return {true, "World-writable file: " + filepath};
    }
  }

  closedir(dp);
  return {false, ""};
}

// --------------------------------------------------------------
// Scan PATH entries and log vulnerabilities, including duplicate executables
// --------------------------------------------------------------
int get_path_vulnerabilities(const std::vector<std::string> &paths) {
  std::ofstream report("PATH.txt", std::ios::app);
  if (!report.is_open() && VERBOSE) {
    cout << "[PATH] WARNING: Could not open PATH.txt for writing." << endl;
  }
  int problems = 0;

  // Map to track executable names -> first directory seen
  std::unordered_map<std::string, std::string> exe_map;

  for (const auto &dir : paths) {
    if (VERBOSE) {
      cout << "[PATH] Scanning directory: " << (dir.empty() ? "." : dir) << endl;
      write_log("PATH.txt", "[DEBUG] Scanning: " + (dir.empty() ? std::string(".") : dir));
    }

    PathVulnResult result = pathutil_dir_has_vulnerability(dir);
    if (result.vulnerable) {
      report << "[!] " << result.reason << " (" << (dir.empty() ? "." : dir) << ")\n";
      problems++;
      if (VERBOSE) {
        cout << "[PATH] Vulnerability: " << result.reason << " (" << (dir.empty() ? "." : dir) << ")" << endl;
      }
    }

    // Scan executables for duplicates
    std::string effective = dir.empty() ? "." : dir;
    DIR *dp = opendir(effective.c_str());
    if (!dp) {
      if (VERBOSE) {
        cout << "[PATH] Could not open path directory for scanning: " << effective << endl;
        write_log("PATH.txt", "[DEBUG] Could not open dir for scanning: " + effective);
      }
      continue;
    }

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
          if (VERBOSE) {
            cout << "[PATH] Duplicate executable: " << exe_name << " in " << effective << " (also in " << exe_map[exe_name] << ")" << endl;
            write_log("PATH.txt", "[DEBUG] Duplicate executable: " + exe_name + " in " + effective + " (also in " + exe_map[exe_name] + ")");
          }
        } else {
          exe_map[exe_name] = effective;
        }
      }
    }

    closedir(dp);
  }

  report.close();

  if (VERBOSE) {
    cout << "[PATH] Completed PATH vulnerability scan. Problems: " << problems << endl;
    write_log("PATH.txt", "[DEBUG] Completed PATH vulnerability scan. Problems: " + std::to_string(problems));
  }

  return problems;
}

// --------------------------------------------------------------
// Utility functions
// --------------------------------------------------------------
void ensure_dir(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        mkdir(path.c_str(), 0755);
        if (VERBOSE) {
            cout << "[FS] Created directory: " << path << endl;
            write_log("general.log", "[DEBUG] Created directory: " + path);
        }
    } else {
        if (VERBOSE) {
            cout << "[FS] Directory exists: " << path << endl;
            write_log("general.log", "[DEBUG] Directory exists: " + path);
        }
    }
}

void list_files_recursive(const std::string& base, std::vector<std::string>& files) {
    DIR* dir = opendir(base.c_str());
    if (!dir) {
        if (VERBOSE) {
            cout << "[FS] Could not open directory for recursion: " << base << endl;
            write_log("general.log", "[DEBUG] Could not open directory: " + base);
        }
        return;
    }

    if (VERBOSE) {
        cout << "[FS] Recursing into: " << base << endl;
        write_log("general.log", "[DEBUG] Recursing into: " + base);
    }

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
                if (VERBOSE) {
                    cout << "[FS] Found file: " << fullpath << endl;
                    write_log("general.log", "[DEBUG] Found file: " + fullpath);
                }
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
        write_log(logDir, "[SUDO] Checking for passwordless sudo access...");
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
    } else {
        if (VERBOSE) {
            cout << "[SUDO] No /etc/sudoers.d directory or unable to open." << endl;
            write_log(logFile, "[SUDO] No /etc/sudoers.d directory or unable to open.");
        }
    }

    for (size_t i = 0; i < sudoFiles.size(); ++i) {

        if (VERBOSE) {
            cout << "[SUDO] Scanning: " << sudoFiles[i] << endl;
            write_log(logFile, "[SUDO] Scanning: " + sudoFiles[i]);
        }

        ifstream in(sudoFiles[i].c_str());
        if (!in.is_open()) {
            if (VERBOSE) {
                cout << "[SUDO] Could not open: " << sudoFiles[i] << endl;
                write_log(logFile, "[SUDO] Could not open: " + sudoFiles[i]);
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
                    write_log(logFile, "[SUDO] Passwordless entry found: " + line);
                }

                write_log(logFile, "Passwordless sudo entry found in: " + sudoFiles[i]);
                write_log(logFile, "  -> " + line);
            }
        }
    }

    if (!found) {
        write_log(logFile, "No passwordless sudo entries found.");
        if (VERBOSE) {
            cout << "[SUDO] No passwordless sudo entries found." << endl;
        }
    }

    if (VERBOSE) {
        cout << "[SUDO] Sudo audit complete." << endl;
        write_log(logFile, "[SUDO] Sudo audit complete.");
    }

    return found;
}

// --------------------------------------------------------------
// 2. World-writable SSH keys
// --------------------------------------------------------------
bool world_writable_ssh_keys(const std::string& logDir) {

    if (VERBOSE) {
        cout << "[SSH] Checking SSH key permissions..." << endl;
        write_log(logDir, "[SSH] Checking SSH key permissions...");
    }

    std::string logFile = logDir;
    write_log(logFile, "=== Checking for world-writable SSH keys ===");

    bool found = false;
    std::vector<std::string> sshDirs;

    const char* home = getenv("HOME");
    if (home) {
        sshDirs.push_back(std::string(home) + "/.ssh");
    } else {
        if (VERBOSE) {
            cout << "[SSH] HOME not set; skipping user .ssh" << endl;
            write_log(logFile, "[SSH] HOME not set; skipping user .ssh");
        }
    }
    sshDirs.push_back("/etc/ssh");

    for (size_t i = 0; i < sshDirs.size(); ++i) {

        if (VERBOSE) {
            cout << "[SSH] Scanning directory: " << sshDirs[i] << endl;
            write_log(logFile, "[SSH] Scanning directory: " + sshDirs[i]);
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
                        write_log(logFile, "[SSH] World-writable SSH file: " + files[j]);
                    }

                    write_log(logFile, "World-writable SSH file: " + files[j]);
                }
            } else {
                if (VERBOSE) {
                    cout << "[SSH] Could not stat file: " << files[j] << endl;
                    write_log(logFile, "[SSH] Could not stat file: " + files[j]);
                }
            }
        }
    }

    if (!found) {
        write_log(logFile, "No world-writable SSH keys found.");
        if (VERBOSE) {
            cout << "[SSH] No world-writable SSH keys found." << endl;
            write_log(logFile, "[SSH] No world-writable SSH keys found.");
        }
    }

    if (VERBOSE) {
        cout << "[SSH] SSH permissions audit complete." << endl;
        write_log(logFile, "[SSH] SSH permissions audit complete.");
    }

    return found;
}

// --------------------------------------------------------------
// 3. SUID binary audit
// --------------------------------------------------------------
bool suid_binary_audit(const std::string& logDir) {

    if (VERBOSE) {
        cout << "[SUID] Starting SUID scan..." << endl;
        write_log(logDir, "[SUID] Starting SUID scan...");
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
            write_log(logFile, "[SUID] Scanning directory: " + dirs[i]);
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
                        write_log(logFile, "[SUID] SUID binary found: " + files[j]);
                    }

                    write_log(logFile, "SUID binary: " + files[j]);
                }
            } else {
                if (VERBOSE) {
                    cout << "[SUID] Could not lstat: " << files[j] << endl;
                    write_log(logFile, "[SUID] Could not lstat: " + files[j]);
                }
            }
        }
    }

    write_log(logFile, "Total SUID binaries found: " + std::to_string(count));
    if (VERBOSE) {
        cout << "[SUID] Scan complete. Total found: " << count << endl;
        write_log(logFile, "[SUID] Scan complete. Total found: " + std::to_string(count));
    }

    return found;
}

// --------------------------------------------------------------
// Helpers used by later audits
// --------------------------------------------------------------
static std::string exec_cmd(const std::string &cmd) {
    std::array<char, 256> buf{};
    std::string out;
    std::unique_ptr<FILE, int(*)(FILE*)> p(popen(cmd.c_str(), "r"), pclose);
    if (!p) {
        if (VERBOSE) {
            cout << "[EXEC] popen failed for command: " << cmd << endl;
            write_log("general.log", "[EXEC] popen failed for: " + cmd);
        }
        return "";
    }
    while (fgets(buf.data(), buf.size(), p.get())) out += buf.data();
    while (!out.empty() && (out.back() == '\n' || out.back() == '\r')) out.pop_back();
    if (VERBOSE) {
        cout << "[EXEC] Command: " << cmd << " => output len: " << out.size() << endl;
        write_log("general.log", "[EXEC] Command: " + cmd + " => output len: " + std::to_string(out.size()));
    }
    return out;
}

static bool stat_path(const std::string &p, uid_t &u, gid_t &g, mode_t &m) {
    struct stat st; if (stat(p.c_str(), &st)) {
        if (VERBOSE) {
            cout << "[STAT] stat failed for: " << p << endl;
            write_log("general.log", "[STAT] stat failed for: " + p);
        }
        return false;
    }
    u = st.st_uid; g = st.st_gid; m = st.st_mode; return true;
}

static bool is_writable_by_nonroot(const std::string &p) {
    uid_t u; gid_t g; mode_t m; if (!stat_path(p,u,g,m)) {
        if (VERBOSE) {
            cout << "[PERM] stat_path failed for: " << p << endl;
            write_log("general.log", "[PERM] stat_path failed for: " + p);
        }
        return false;
    }
    bool writable = ((m & S_IWUSR && u != 0) || (m & S_IWGRP && g != 0) || (m & S_IWOTH));
    if (VERBOSE) {
        cout << "[PERM] Path: " << p << " writable_by_nonroot: " << (writable ? "yes" : "no") << endl;
        write_log("general.log", "[PERM] " + p + " writable_by_nonroot: " + (writable ? "yes" : "no"));
    }
    return writable;
}

static std::string md5(const std::string &p) {
    std::string cmd = "md5sum '" + p + "' 2>/dev/null | awk '{print $1}'";
    std::string res = exec_cmd(cmd);
    if (VERBOSE) {
        cout << "[MD5] " << p << " => " << (res.empty() ? "(none)" : res) << endl;
        write_log("general.log", "[MD5] " + p + " => " + (res.empty() ? "(none)" : res));
    }
    return res;
}

static void log_line(const std::string &f, const std::string &l) {
    // Mirror to logfile and to stdout when VERBOSE
    write_log(f, l);
}

// --------------------------------------------------------------
// SUID / Package ownership audit
// --------------------------------------------------------------
bool suid_package_audit(const std::string &log) {
    bool issues=false;
    log_line(log,"=== SUID/SGID package audit ===");
    if (VERBOSE) {
        cout << "[SUID-PKG] Starting SUID/SGID package audit..." << endl;
        write_log(log, "[SUID-PKG] Starting SUID/SGID package audit...");
    }

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
            log_line(log,ev.str()+" | pkg:(none) => suspicious");
            issues=true;
            if (VERBOSE) {
                cout << "[SUID-PKG] Suspicious SUID/SGID file with no package: " << f << endl;
                write_log(log, "[SUID-PKG] Suspicious file: " + f);
            }
            continue;
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
        if(bad) {
            issues=true;
            if (VERBOSE) {
                cout << "[SUID-PKG] Modified SUID/SGID file: " << f << " pkg: " << pkg << endl;
                write_log(log, "[SUID-PKG] Modified file: " + f + " pkg: " + pkg);
            }
        } else {
            if (VERBOSE) {
                write_log(log, "[SUID-PKG] OK file: " + f + " pkg: " + pkg);
            }
        }
    }
    log_line(log,"=== End SUID/SGID audit ===");
    if (VERBOSE) {
        cout << "[SUID-PKG] Completed SUID/SGID package audit. Issues: " << (issues ? "yes" : "no") << endl;
        write_log(log, "[SUID-PKG] Completed SUID/SGID package audit. Issues: " + std::string(issues ? "yes" : "no"));
    }
    return issues;
}

// --------------------------------------------------------------
// systemd unit audit
// --------------------------------------------------------------
bool systemd_unit_audit(const std::string &log) {
    bool issues=false;
    log_line(log,"=== systemd unit audit ===");
    if (VERBOSE) {
        cout << "[SYSTEMD] Starting systemd unit audit..." << endl;
        write_log(log, "[SYSTEMD] Starting systemd unit audit...");
    }

    std::vector<std::string> dirs={"/etc/systemd/system","/lib/systemd/system","/usr/lib/systemd/system","/run/systemd/system"};
    std::vector<std::string> units;
    for(auto &d:dirs){
        std::istringstream s(exec_cmd("find '"+d+"' -type f \\( -name '*.service' -o -name '*.timer' \\) -print 2>/dev/null"));
        for(std::string u;std::getline(s,u);) if(!u.empty()) units.push_back(u);
    }

    if (VERBOSE) {
        cout << "[SYSTEMD] Found " << units.size() << " units to inspect." << endl;
        write_log(log, "[SYSTEMD] Found " + std::to_string(units.size()) + " units to inspect.");
    }

    for(auto &u:units){
        uid_t uid; gid_t gid; mode_t mode;
        if(stat_path(u,uid,gid,mode)&&uid){
            log_line(log,"non-root owned unit:"+u); issues=true;
            if (VERBOSE) {
                cout << "[SYSTEMD] Non-root owned unit: " << u << " uid: " << uid << endl;
                write_log(log, "[SYSTEMD] Non-root owned unit: " + u);
            }
        }
        auto pos=u.find_last_of('/'); std::string dir=u.substr(0,pos);
        if(is_writable_by_nonroot(dir)){log_line(log,"unit dir writable:"+dir); issues=true;
            if (VERBOSE) {
                cout << "[SYSTEMD] Unit dir writable: " << dir << " for unit " << u << endl;
                write_log(log, "[SYSTEMD] Unit dir writable: " + dir + " for unit " + u);
            }
        }
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
                if (VERBOSE) {
                    cout << "[SYSTEMD] Risky Exec: " << exe << " in unit " << u << endl;
                    write_log(log, "[SYSTEMD] Risky Exec: " + exe + " in unit " + u);
                }
            }
        }
    }
    log_line(log,"=== End systemd audit ===");
    if (VERBOSE) {
        cout << "[SYSTEMD] systemd unit audit complete. Issues: " << (issues ? "yes" : "no") << endl;
        write_log(log, "[SYSTEMD] systemd unit audit complete. Issues: " + std::string(issues ? "yes" : "no"));
    }
    return issues;
}

