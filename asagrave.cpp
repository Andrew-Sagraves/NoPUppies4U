#include "asagrave.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <unordered_map>

// Retrieve PATH entries as a vector of strings
std::vector<std::string> get_paths() {
    std::vector<std::string> paths;
    const char* path_env = getenv("PATH");
    if (!path_env) return paths;

    std::string path_str(path_env);
    std::stringstream ss(path_str);
    std::string dir;
    while (std::getline(ss, dir, ':')) {
        paths.push_back(dir); // keep empty entries (treated as ".")
    }
    return paths;
}

// Check if a directory (and its parents) are world-writable
bool pathutil_is_world_writable_dir(const std::string& dir) {
    if (dir.empty()) return false;

    std::string path = dir;

    // Remove trailing slashes
    while (path.length() > 1 && path.back() == '/') {
        path.pop_back();
    }

    struct ::stat st;

    while (!path.empty()) {
        if (stat(path.c_str(), &st) == -1) return false;
        if (S_ISDIR(st.st_mode) && (st.st_mode & S_IWOTH)) return true;

        if (path == "/") break; // root reached

        auto pos = path.find_last_of('/');
        if (pos == std::string::npos) break;

        if (pos == 0) path = "/";
        else path = path.substr(0, pos);
    }

    return false;
}

// Check if directory string is relative
bool pathutil_is_relative_dir(const std::string& dir) {
    if (dir.empty() || dir == ".") return true;
    return dir[0] != '/';
}

// Check if a directory or its files have vulnerabilities
PathVulnResult pathutil_dir_has_vulnerability(const std::string& dir) {
    std::string effective = dir.empty() ? "." : dir;

    if (pathutil_is_relative_dir(effective)) {
        return {true, "Relative directory"};
    }

    if (pathutil_is_world_writable_dir(effective)) {
        return {true, "World-writable directory (or parent)"};
    }

    // Scan files inside the directory for world-writable files
    DIR* dp = opendir(effective.c_str());
    if (!dp) return {false, ""};

    struct dirent* entry;
    struct ::stat st;
    std::string filepath;

    while ((entry = readdir(dp)) != nullptr) {
        filepath = effective + "/" + entry->d_name;
        if (stat(filepath.c_str(), &st) == -1) continue;
        if (S_ISREG(st.st_mode) && (st.st_mode & S_IWOTH)) {
            closedir(dp);
            return {true, "World-writable file: " + filepath};
        }
    }

    closedir(dp);
    return {false, ""};
}

// Scan PATH entries and log vulnerabilities, including duplicate executables
int get_path_vulnerabilities(const std::vector<std::string>& paths) {
    std::ofstream report("PATH.txt");
    int problems = 0;

    // Map to track executable names -> first directory seen
    std::unordered_map<std::string, std::string> exe_map;

    for (const auto& dir : paths) {
        PathVulnResult result = pathutil_dir_has_vulnerability(dir);
        if (result.vulnerable) {
            report << "[!] " << result.reason << " (" 
                   << (dir.empty() ? "." : dir) << ")\n";
            problems++;
        }

        // Scan executables for duplicates
        std::string effective = dir.empty() ? "." : dir;
        DIR* dp = opendir(effective.c_str());
        if (!dp) continue;

        struct dirent* entry;
        struct ::stat st;
        std::string filepath;

        while ((entry = readdir(dp)) != nullptr) {
            filepath = effective + "/" + entry->d_name;
            if (stat(filepath.c_str(), &st) == -1) continue;

            if (S_ISREG(st.st_mode) && (st.st_mode & S_IXUSR)) {
                std::string exe_name = entry->d_name;
                if (exe_map.find(exe_name) != exe_map.end()) {
                    report << "[!] Duplicate executable: " << exe_name
                           << " found in " << effective
                           << " (also in " << exe_map[exe_name] << ")\n";
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

