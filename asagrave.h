#ifndef ASAGRAVE_H
#define ASAGRAVE_H

#include <string>
#include <vector>
#include <unordered_map>

// Retrieve the $PATH entries as a vector of strings
std::vector<std::string> get_paths();

// Check if a directory (and its parents) are world-writable
bool pathutil_is_world_writable_dir(const std::string& dir);

// Check if directory string is relative (".", empty, or not starting with '/')
bool pathutil_is_relative_dir(const std::string& dir);

// Struct to hold vulnerability result
struct PathVulnResult {
    bool vulnerable;
    std::string reason;
};

// Check if a specific directory has a vulnerability (directory or files)
PathVulnResult pathutil_dir_has_vulnerability(const std::string& dir);

// Scan PATH entries and log vulnerabilities to PATH.txt
// Returns the number of problems found
int get_path_vulnerabilities(const std::vector<std::string>& paths);

bool passwordless_sudo_access(const std::string& logDir);
bool world_writable_ssh_keys(const std::string& logDir);
bool suid_binary_audit(const std::string& logDir);

#endif // ASAGRAVE_H

