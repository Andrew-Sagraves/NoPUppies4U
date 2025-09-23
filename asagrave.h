#ifndef ASAGRAVE_H
#define ASAGRAVE_H 

#include <vector>

// calls the $PATH system variable and returns the directories inside $PATH as a vector of strings
std::vector<std::string> get_paths();


// takes in a vector of strings and outputs a return code along with a file output if necesary
int get_path_vulnerabilities(std::vector<std::string>);

// determines if a specific directory has a path vulneraility
bool path_directory_has_vulnerability(std::string);

// takes in a directory and determins if it is world-writeable
bool is_world_writable(std::string);
#endif
