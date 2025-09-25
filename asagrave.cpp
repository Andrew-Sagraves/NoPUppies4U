#include "asagrave.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

// calls the $PATH system variable and returns the directories inside $PATH as a vector of strings
std::vector<std::string> get_paths() {
    std::vector<std::string> paths;

    const char* path_env = getenv("PATH");
    if (!path_env) {
        return paths; // return empty if PATH is unset
    }

    std::string path_str(path_env);
    std::stringstream ss(path_str);
    std::string dir;

    while (std::getline(ss, dir, ':')) {
        // keep empty entries (treated as ".")
        paths.push_back(dir);
    }

    return paths;
}

// Check if a directory (and its parents) are world-writable
bool pathutil_is_world_writable_dir(const std::string& dir) {
    std::string path = dir;
    struct ::stat st;

    while (!path.empty()) {
        if (stat(path.c_str(), &st) == -1) {
            return true; // can't stat -> treat as unsafe
        }
        if (S_ISDIR(st.st_mode) && (st.st_mode & S_IWOTH)) {
            return true;
        }

        // Go one level up
        auto pos = path.find_last_of('/');
        if (pos == std::string::npos) break;
        if (pos == 0) {
            path = "/";
        } else {
            path = path.substr(0, pos);
        }
    }

    return false;
}

// Check if directory string is relative (".", empty, or not starting with '/')
bool pathutil_is_relative_dir(const std::string& dir) {
    if (dir.empty() || dir == ".") return true;
    return dir[0] != '/';
}

// Check if a specific directory has a vulnerability
bool pathutil_dir_has_vulnerability(const std::string& dir) {
    std::string effective = dir.empty() ? "." : dir;

    if (pathutil_is_relative_dir(effective)) {
        return true;
    }

    if (pathutil_is_world_writable_dir(effective)) {
        return true;
    }

    DIR* dp = opendir(effective.c_str());
    if (!dp) return false;

    struct dirent* entry;
    struct ::stat st;
    std::string filepath;

    while ((entry = readdir(dp)) != nullptr) {
        filepath = effective + "/" + entry->d_name;
        if (stat(filepath.c_str(), &st) == -1) continue;
        if (S_ISREG(st.st_mode) && (st.st_mode & S_IWOTH)) {
            closedir(dp);
            return true;
        }
    }

    closedir(dp);
    return false;
}

// Scan PATH entries and log vulnerabilities to PATH.txt
int get_path_vulnerabilities(const std::vector<std::string>& paths) {
    std::ofstream report("PATH.txt");
    int problems = 0;

    for (const auto& dir : paths) {
        if (pathutil_dir_has_vulnerability(dir)) {
            report << "[!] Vulnerable PATH entry: "
                   << (dir.empty() ? "." : dir) << "\n";
            problems++;
        }
    }

    report.close();
    return problems;
}

