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
    bool verbose = false;
};

//Loads original date modified times from a file for comparison into a map
map<string, time_t> load_previous_date_modified(const string& dateModifiedFile);
//Recursively finds and stores all files in a directory and subdirectories in a vector
vector<string> get_all_files_recursively(const string& directoryPath, bool ignoreHidden = false);
//All in one function to create a date modified file and check against that file on subsequent runs
void check_directory_for_changes(const string& checkingDirectory, const DirectoryCheckFlags& flags = DirectoryCheckFlags());

//Parses a specified log file for keywords and saves matching lines to a report.
void parse_log_file(const string& logFilePath, const vector<string>& keywords, const string& reportFile);
//Parses /var/log/syslog for specific keywords.
void parse_system_logs(const vector<string>& keywords, const string& reportFile);
//Parses /var/log/kern.log for specific keywords.
void parse_kernel_logs(const vector<string>& keywords, const string& reportFile);
//Parses /var/log/auth.log for specific keywords.
void parse_authentication_logs(const vector<string>& keywords, const string& reportFile);
//Parses /var/log/app.log for specific keywords.
void parse_application_logs(const vector<string>& keywords, const string& reportFile);

#endif