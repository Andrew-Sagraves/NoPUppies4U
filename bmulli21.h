#ifndef BMULLI21_H
#define BMULLI21_H

#include <string>
#include <vector>
#include <map>
#include <ctime>

using namespace std;

//Struct to hold flags for the directory check function
struct DirectoryCheckFlags {
    bool rootCheck = false;
    bool writeNew = false;
    bool ignoreHidden = false;
};

// Loads previous date modified times from a file into a map.
// Parameters:
//     dateModifiedFile - path to the stored timestamps file.
// Returns:
//     map of file path -> last modification time.
map<string, time_t> load_previous_date_modified(const string& dateModifiedFile);
// Recursively finds and returns all regular files in a directory and its subdirectories.
// Parameters:
//     directoryPath - root path to scan.
//     ignoreHidden - if true, skip hidden files and directories.
// Returns:
//     vector of file paths found.
vector<string> get_all_files_recursively(const string& directoryPath, bool ignoreHidden = false);
// Check a directory for changed files, update stored timestamps, and optionally create a report.
// Parameters:
//     checkingDirectory - path to check.
//     flags - behavior flags (rootCheck, writeNew, ignoreHidden).
void check_directory_for_changes(const string& checkingDirectory, const DirectoryCheckFlags& flags = DirectoryCheckFlags());

// Parse the given log file for any of the provided keywords and write matching lines to reportFile.
// Parameters:
//     logFilePath - path to the log file to parse.
//     keywords - list of keywords to search for.
//     reportFile - output file to store matching lines.
void parse_log_file(const string& logFilePath, const vector<string>& keywords, const string& reportFile);

// Parse the system log (/var/log/syslog) for keywords and write to reportFile.
void parse_system_logs(const vector<string>& keywords, const string& reportFile);

// Parse the kernel log (/var/log/kern.log) for keywords and write to reportFile.
void parse_kernel_logs(const vector<string>& keywords, const string& reportFile);

// Parse the authentication log (/var/log/auth.log) for keywords and write to reportFile.
void parse_authentication_logs(const vector<string>& keywords, const string& reportFile);

// Parse the application log (/var/log/app.log) for keywords and write to reportFile.
void parse_application_logs(const vector<string>& keywords, const string& reportFile);

// Parse all major logs for keywords and save to respective reports.
void parse_all_logs(const vector<string>& keywords, const string& reportFile);

#endif