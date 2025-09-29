#ifndef KBISSONN_H
#define KBISSONN_H

using namespace std;

#include <string>
#include <vector>
#include <filesystem>
#include <set>

//helper functions for check_cron_jobs()
bool is_schedule_token(const string &t);
vector<string> split_tokens(const string &s);
string sanitize_token(string t);
vector<string> extract_paths(const string &cmd);
void process_file(const filesystem::path &p, set<string> &checked_files);

void check_cron_jobs();

// For verbose
void list_cronjob_commands();


//void check_sudoers();

#endif