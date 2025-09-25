#ifndef ASAGRAVE_H
#define ASAGRAVE_H

#include <string>
#include <vector>

// Retrieve the $PATH entries as a vector of strings
std::vector<std::string> get_paths();

// Check if a directory (and its parents) are world-writable
bool pathutil_is_world_writable_dir(const std::string& dir);

// Check if a directory string is relative (".", empty, or not starting with '/')
bool pathutil_is_relative_dir(const std::string& dir);

// Check if a specific directory has a vulnerability
bool pathutil_dir_has_vulnerability(const std::string& dir);

// Scan PATH entries and log vulnerabilities to PATH.txt
// Returns the number of problems found
int get_path_vulnerabilities(const std::vector<std::string>& paths);

#endif // ASAGRAVE_H

