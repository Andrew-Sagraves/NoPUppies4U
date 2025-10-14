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

#include <string>
#include <vector>

/*
 * ==============================================================
 *  security_audit.h
 *  -----------------
 *  Function prototypes for system audit utilities.
 *  These check for:
 *      1. Passwordless sudo access
 *      2. World-writable SSH keys
 *      3. SUID binaries
 *
 *  Author:  (Your Name)
 *  Date:    (Today's Date)
 *  Version: 1.0
 *
 *  Notes:
 *   - Fully portable (no <filesystem>, C++03 compatible)
 *   - Intended for Linux or POSIX-like systems
 *   - All results are logged under /tmp/security_audit_logs/
 * ==============================================================
 */

// --------------------------------------------------------------
// Utility functions (helpers used internally)
// --------------------------------------------------------------

/**
 * @brief Ensures a directory exists (creates it if necessary)
 * @param path Directory path
 */
void ensure_dir(const std::string& path);

/**
 * @brief Appends a message to a log file
 * @param file  Path to log file
 * @param message  Line of text to write
 */
void write_log(const std::string& file, const std::string& message);

/**
 * @brief Recursively collects all regular files in a directory
 * @param base  Root directory path
 * @param files Reference to vector to store file paths
 */
void list_files_recursive(const std::string& base, std::vector<std::string>& files);


// --------------------------------------------------------------
// 1. Passwordless sudo access
// --------------------------------------------------------------

/**
 * @brief Checks for passwordless sudo access entries in /etc/sudoers and /etc/sudoers.d
 * @param logDir Directory path where log file will be written
 * @return true if passwordless sudo access entries are found
 */
bool passwordless_sudo_access(const std::string& logDir);


// --------------------------------------------------------------
// 2. World-writable SSH keys
// --------------------------------------------------------------

/**
 * @brief Checks for SSH key files with world-writable permissions
 * @param logDir Directory path where log file will be written
 * @return true if any world-writable SSH keys are found
 */
bool world_writable_ssh_keys(const std::string& logDir);


// --------------------------------------------------------------
// 3. SUID binary audit
// --------------------------------------------------------------

/**
 * @brief Scans common system directories for SUID binaries
 * @param logDir Directory path where log file will be written
 * @return true if any SUID binaries are found
 */
bool suid_binary_audit(const std::string& logDir);


#endif // ASAGRAVE_H

