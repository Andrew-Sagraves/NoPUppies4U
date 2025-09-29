#ifndef BMULLI21_H
#define BMULLI21_H

#include <string>
#include <vector>
#include <map>
#include <ctime>

using namespace std;

//Loads original date modified times from a file for comparison into a map
map<string, time_t> load_previous_date_modified(const string& dateModifiedFile);
//Recursively finds and stores all files in a directory and subdirectories in a vector
vector<string> get_all_files_recursively(const string& directoryPath);
//All in one function to create a date modified file and check against that file on subsequent runs
void check_directory_for_changes(const string& checkingDirectory);

#endif