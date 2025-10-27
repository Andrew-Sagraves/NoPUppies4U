#ifndef ASAGRAVE_H
#define ASAGRAVE_H

#include <string>
#include <vector>
#include <unordered_map>

/*
 * Utility / path helpers
 */

/**
 * Retrieve the $PATH entries as a vector of strings (ordered left-to-right).
 */
std::vector<std::string> get_paths();

/**
 * Check if a directory (and its parents, as appropriate) is world-writable.
 * Returns true if the directory is writable by "others".
 */
bool pathutil_is_world_writable_dir(const std::string& dir);

/**
 * Check if directory string is relative (i.e., empty, "." or not starting with '/').
 */
bool pathutil_is_relative_dir(const std::string& dir);

/**
 * Struct to hold a directory vulnerability result
 */
struct PathVulnResult {
    bool vulnerable;       // true if a vulnerability is present
    std::string reason;    // short human-friendly explanation
};

/**
 * Check if a specific directory has a vulnerability (e.g., world-writable, insecure perms).
 * Returns PathVulnResult with explanation.
 */
PathVulnResult pathutil_dir_has_vulnerability(const std::string& dir);

/**
 * Scan PATH entries and log vulnerabilities to PATH.txt (or other configured sink).
 * Returns the number of problems found (0 = none).
 */
int get_path_vulnerabilities(const std::vector<std::string>& paths);


/*
 * Existing/high-level audits (already present in your project)
 */

/**
 * Detects users that can run sudo without a password (NOPASSWD entries).
 * Writes findings to logDir and returns true if any issues are found.
 */
bool passwordless_sudo_access(const std::string& logDir);

/**
 * Detect world-writable or incorrectly permissioned SSH keys under user home directories.
 * Writes findings to logDir and returns true if any issues are found.
 */
bool world_writable_ssh_keys(const std::string& logDir);

/**
 * (Older SUID scan) Scan for SUID/SGID binaries and perform a basic audit.
 * Writes findings to logDir and returns true if any issues are found.
 */
bool suid_binary_audit(const std::string& logDir);


/*
 * New audits added — prototypes for the functions you asked to include
 */

/**
 * suid_package_audit
 *
 * Detect SUID/SGID files, determine package ownership (dpkg or rpm), compute local
 * checksum and compare to package metadata (Debian md5sums or RPM verification).
 *
 * - Arguments:
 *     logFile : path to the plaintext log file where findings will be appended.
 * - Behavior:
 *     read-only enumeration; uses dpkg/rpm and md5sum/sha256sum for verification.
 * - Returns:
 *     true if suspicious items (unowned SUIDs or checksum mismatches) were found.
 */
bool suid_package_audit(const std::string& logFile);

/**
 * systemd_unit_audit
 *
 * Parse installed .service and .timer files in standard systemd locations and look for:
 *  - ExecStart/ExecStartPre/ExecStartPost referencing executables in writable directories,
 *  - Unit files that are owned by non-root,
 *  - Unit directories or drop-in (.d\*.conf) files that are writable or owned by non-root.
 *
 * - Arguments:
 *     logFile : path to the plaintext log file where findings will be appended.
 * - Behavior:
 *     read-only parsing of unit files and filesystem metadata checks.
 * - Returns:
 *     true if misconfigurations or risky items were found.
 */
bool systemd_unit_audit(const std::string& logFile);


#endif // ASAGRAVE_H

