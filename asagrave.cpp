#include "asagrave.h"
#include <dirent.h>
#include <filesystem>
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

// Create a directory if it doesnÃ¢ÂÂt exist
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
    std::string logFile = logDir;
    write_log(logFile, "=== Checking for passwordless SUDO access ===");

    bool found = false;
    const char* username = getenv("USER");
    if (!username) username = "UNKNOWN";

    std::vector<std::string> sudoFiles;
    sudoFiles.push_back("/etc/sudoers");

    // Include any files under /etc/sudoers.d
    DIR* d = opendir("/etc/sudoers.d");
    if (d) {
        struct dirent* e;
        while ((e = readdir(d)) != NULL) {
            if (e->d_name[0] == '.') continue;
            sudoFiles.push_back(std::string("/etc/sudoers.d/") + e->d_name);
        }
        closedir(d);
    }

    for (size_t i = 0; i < sudoFiles.size(); ++i) {
        std::ifstream in(sudoFiles[i].c_str());
        if (!in.is_open()) continue;

        std::string line;
        while (std::getline(in, line)) {
            if (line.find("NOPASSWD") != std::string::npos &&
                (line.find(username) != std::string::npos || line.find("ALL") != std::string::npos)) {
                found = true;
                write_log(logFile, "Passwordless sudo entry found in: " + sudoFiles[i]);
                write_log(logFile, "  -> " + line);
            }
        }
    }

    if (!found)
        write_log(logFile, "No passwordless sudo entries found.");

    return found;
}

// --------------------------------------------------------------
// 2. World-writable SSH keys
// --------------------------------------------------------------
bool world_writable_ssh_keys(const std::string& logDir) {
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
        std::vector<std::string> files;
        list_files_recursive(sshDirs[i], files);
        for (size_t j = 0; j < files.size(); ++j) {
            struct stat st;
            if (stat(files[j].c_str(), &st) == 0) {
                if (st.st_mode & S_IWOTH) {
                    found = true;
                    write_log(logFile, "World-writable SSH file: " + files[j]);
                }
            }
        }
    }

    if (!found)
        write_log(logFile, "No world-writable SSH keys found.");

    return found;
}

// --------------------------------------------------------------
// 3. SUID binary audit
// --------------------------------------------------------------
bool suid_binary_audit(const std::string& logDir) {
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
        std::vector<std::string> files;
        list_files_recursive(dirs[i], files);
        for (size_t j = 0; j < files.size(); ++j) {
            struct stat st;
            if (lstat(files[j].c_str(), &st) == 0) {
                if (st.st_mode & S_ISUID) {
                    found = true;
                    count++;
                    write_log(logFile, "SUID binary: " + files[j]);
                }
            }
        }
    }

    write_log(logFile, "Total SUID binaries found: " + std::to_string(count));
    return found;
}

