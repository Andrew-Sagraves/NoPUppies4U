#include "asagrave.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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


// Check if directory string is relative (".", empty, or not starting with '/')
bool pathutil_is_relative_dir(const std::string& dir) {
    if (dir.empty() || dir == ".") return true;
    return dir[0] != '/';
}

// Check if a specific directory has a vulnerability and return reason
PathVulnResult pathutil_dir_has_vulnerability(const std::string& dir) {
    std::string effective = dir.empty() ? "." : dir;

    if (pathutil_is_relative_dir(effective)) {
        return {true, "Relative directory"};
    }

    if (pathutil_is_world_writable_dir(effective)) {
        return {true, "World-writable directory (or parent)"};
    }

    return {false, ""}; // no vulnerability
}

// Scan PATH entries and log vulnerabilities to PATH.txt
int get_path_vulnerabilities(const std::vector<std::string>& paths) {
    std::ofstream report("PATH.txt");
    int problems = 0;

    for (const auto& dir : paths) {
        PathVulnResult result = pathutil_dir_has_vulnerability(dir);
        if (result.vulnerable) {
            report << "[!] " << result.reason << " (" << (dir.empty() ? "." : dir) << ")\n";
            problems++;
        }
    }

    report.close();
    return problems;
}

