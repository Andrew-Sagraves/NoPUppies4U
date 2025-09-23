#ifndef BMULLI21_H
#define BMULLI21_H

#include <vector>
#include <string>

using namespace std;

//Functions have been made void to avoid conflicts with other files

//Returns true if a file has been modified since the last check
//bool check_date_modified(string filename);
void check_date_modified(string filename);
//Creates a file with all the files that have been modified since the last check
void create_file(vector<string> files); 
//Creates a vector of all files in a directory
//vector<string> create_vector(string directory);
void create_vector(string directory);
//Checks if any files in a directory have been modified since the last check
void check_directory(vector<string> files);
//Recursively checks a directory and subdirectories for modified files
void check_all_directories(string directory);


#endif