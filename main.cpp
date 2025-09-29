#include <iostream>
#include <string.h>
#include <stdio.h>
#include <string>
//#include "main.hpp"
//#include "asagrave.h"
#include <getopt.h> 
#include "asagrave.h"
#include "bmulli21.h"
#include "jdong11.h"
#include "kbissonn.h"
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
		{"crontab", no_argument,	0, 'c'},
		{"path", no_argument,		0, 'p'},
		{"sources", no_argument,	0, 'x'},
		{"all", no_argument,		0, 'a'},
		{"directory", required_argument,	0, 'd'},
		{0, 0, 0, 0}
	};
	
	int opt = 0;
	int options_index = 0;
	
	if (argc == 1) {
		cout << "Usage: nopuppies4u [options]" << endl;
		return 0;
	}
	
	while ((opt = getopt_long(argc, argv, "hcpxad:", long_options, &options_index)) != -1) { //hvscp lets short options work, like -s
		//you have to make sure to add any additional options to that ""
		
		switch (opt) {
			case 'h':
				cout << "Usage: nopuppies4u [options]" << endl;
				cout << "Options:" << endl;

				cout << "  " << left << setw(25) << "-h,   --help"    << "Show this help message" << endl;
				cout << "  " << left << setw(25) << "-c,   --crontab" << "Check crontab" << endl;
				cout << "  " << left << setw(25) << "-x,   --sources" << "Check all sources" << endl;
				cout << "  " << left << setw(25) << "-p,   --path"    << "Check path" << endl;
				cout << "  " << left << setw(25) << "-a,   --all"     << "Run all tests" << endl;
				cout << "  " << left << setw(25) << "-d,   --directory"     << "Check directory for changes" << endl;
				return 0;
				break;
				
			case 'a': {
				//add all your functions here- this is the "all" option
				
				check_sources_list();
			
				vector<string> paths = get_paths();
				int problems = get_path_vulnerabilities(paths);
				cout << "PATH scan complete. " << problems << " potential issue(s) found. Issues outputted to PATH.txt\n";
			
				check_cron_jobs();
				
				//i'm choosing to not include directory case in this because it would require the --all flag to take an argument, which wouldn't really work if another one required an argument as well
				
				return 0;
				break;
			}
			case 'x':
				check_sources_list();
				break;
			
			case 'd': {
				if (optarg == nullptr) {
					cerr << "Error: --directory requires an argument" << endl;
					return 1;
				}
				string directory = optarg;
				check_directory_for_changes(directory);
				break;
			}
			case 'c':
				check_cron_jobs();
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
