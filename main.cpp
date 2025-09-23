#include <iostream>
#include <string.h>
#include <stdio.h>
//#include "main.hpp"
//#include "asagrave.h"
#include <getopt.h> 

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

		{0, 0, 0, 0}
	};
	
	int opt = 0;
	int options_index = 0;
	
	while ((opt = getopt_long(argc, argv, "", long_options, &options_index)) != -1) {
		switch (opt) {
			case 'h':
				cout << "Usage: nopuppies4u [options]" << endl;
				cout << "Options:" << endl;
				cout << "	-h,   --help		Show this help message" << endl;
				cout << "	-v,   --verbose 		Enable verbose output" << endl;
				cout << "	-c,   --crontab		Check crontab" << endl;
				cout << "	-s,   --sudo		Check sudo permissions" << endl;
				cout << "	-p,   --path		Check path" << endl;
				return 0;
				break;
			case 'v':
			
				break;
			case 'o':
				
				break;
			case 's':
				
				break;
			case 'c':
				
				break;
			case 'p':
			
				break;
			case '?': //apparently occurs when it gets an unknown flag? 
				cerr << "Error: invalid argument '" << argv[optind - 1] << "'\n";  
			default:
				return 1;
		}
	}
	
	return 0;
}
