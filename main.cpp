#include <iostream>
#include <string.h>
#include <stdio.h>
//#include "main.hpp"
//#include "asagrave.h"
#include <getopt.h> 
#include "asagrave.h"
#include "bmulli21.h"
#include "jdong11.h"
#include <vector>
#include <iomanip>

using namespace std;

int main(int argc, char* argv[]) {
	
	/*
		- Check to see if argv[i] is a valid --(command)
		- If (command) requires input, check argv[i + 1] for valid input. If so, increment i to skip it for next, and process it, otherwise, error? 
		- Goal: have it always either work or fail. No unexpected states 
		
		-p for path
		-c for crontab
		-s for sudo
		-a for all?
		-v 
	*/
	
	static struct option long_options[] = {
		{"help", no_argument,		0, 'h'},
		{"verbose", no_argument,	0, 'v'},
		{"crontab", no_argument,	0, 'c'},
		{"sudo", no_argument,		0, 's'},
		{"path", no_argument,		0, 'p'},
		{"sources", no_argument,	0, 'x'},
		{"all", no_argument,		0, 'a'},
		{0, 0, 0, 0}
	};
	
	int opt = 0;
	int options_index = 0;
	
	if (argc == 1) {
		cout << "Usage: nopuppies4u [options]" << endl;
		return 0;
	}
	
	while ((opt = getopt_long(argc, argv, "hvscpxa", long_options, &options_index)) != -1) { //hvscp lets short options work, like -s
		//you have to make sure to add any additional options to that ""
		
		switch (opt) {
			case 'h':
				cout << "Usage: nopuppies4u [options]" << endl;
				cout << "Options:" << endl;

				cout << "  " << left << setw(25) << "-h,   --help"    << "Show this help message" << endl;
				cout << "  " << left << setw(25) << "-v,   --verbose" << "Enable verbose output" << endl;
				cout << "  " << left << setw(25) << "-c,   --crontab" << "Check crontab" << endl;
				cout << "  " << left << setw(25) << "-s,   --sudo"    << "Check sudo permissions" << endl;
				cout << "  " << left << setw(25) << "-x,   --sources" << "Check all sources" << endl;
				cout << "  " << left << setw(25) << "-p,   --path"    << "Check path" << endl;
				cout << "  " << left << setw(25) << "-a,   --all"     << "Run all tests" << endl;
				return 0;
				break;
				
			case 'a':
				//add all your functions here- this is the "all" option
				
				check_sources_list();
				return 0;
				break;
			case 'x':
				check_sources_list();
				break;
			case 'v':
				
				break;
			case 'o':
				
				break;
			case 's':
				
				break;
			case 'c':
				
				break;
			case 'p': {
				vector<string> paths = get_paths();
				int problems = get_path_vulnerabilities(paths);
				cout << "PATH scan complete. " << problems << " potential issue(s) found. Issues outputted to PATH.txt\n";
				break;
			}
			case '?': 
				//apparently occurs when it gets an unknown flag? 
				cerr << "Error: invalid argument '" << argv[optind - 1] << "'\n";  
			default:
				return 1;
		}
	}
	
	return 0;
}
