#ifndef KBISSONN_H
#define KBISSONN_H

using namespace std;

#include <string>
#include <vector>
#include <filesystem>
#include <set>


//helper functions for check_cron_jobs()
// Go through and see if it is a schedule token (5 * * * *, @reboot, etc)
bool is_schedule_token(const string &t);
// Split a string into tokens, preserving quoted substrings (mainly to get the path name to pass to extract path)
vector<string> split_tokens(const string &s);
// Removing extra quotes and special chars
string sanitize_token(string t);
// Get everthring that looks like a path from a command string
vector<string> extract_paths(const string &cmd);
// Get everthring that looks like a path from a command string
void process_file(const filesystem::path &p, set<string> &checked_files);

// Checks for cron jobs in /etc/cron.d, /etc/cron.daily, /etc/cron.hourly, /etc/cron.monthly, /etc/cron.weekly
void check_cron_jobs();

// For verbose (prints all function calls from all of the cron jobs to console)
void list_cronjob_commands();


// Looks through the sudoers file and prints out all unique users with sudo permissions
void check_sudoers();

#endif
