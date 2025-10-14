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
#include "hclark37.h"
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
		{"help",          no_argument,       0, 'h'},
		{"crontab",       no_argument,       0, 'c'},
		{"path",          no_argument,       0, 'p'}, 
		{"sudo",          no_argument,       0, 's'}, 
		{"ssh-keys",      no_argument,       0, 'k'},
		{"suid",          no_argument,       0, 'b'},  
		{"all",           no_argument,       0, 'a'},  
		{"directory",     required_argument, 0, 'd'},
		{"root",          no_argument,       0, 'r'},
		{"write-new",     no_argument,       0, 'w'},
		{"ignore-hidden", no_argument,       0, 'i'},
		{"sudo-group",     no_argument, 0, 'g'}, 
		{"system-update",  no_argument, 0, 'U'},
		{0, 0, 0, 0}
	};
	
	int opt = 0;
	int options_index = 0;
	
	if (argc == 1) {
		cout << "Usage: nopuppies4u [options]" << endl;
		return 0;
	}
	
	DirectoryCheckFlags dirFlags;
	
	string logDir = "./"; //may not be hardcoded later
	
	while ((opt = getopt_long(argc, argv, "hrwicpksbagUd:", long_options, &options_index)) != -1) { //hvscp lets short options work, like -s
		//you have to make sure to add any additional options to that ""
		
		switch (opt) {
			case 'h':
				cout << "Usage: nopuppies4u [options]" << endl;
				cout << "Options:" << endl;
				cout << "  " << left << setw(25) << "-h,   --help"           << "Show this help message" << endl;
				cout << "  " << left << setw(25) << "-c,   --crontab"        << "Check crontab" << endl;
				cout << "  " << left << setw(25) << "-p,   --path"           << "Check PATH for vulnerabilities" << endl;
				cout << "  " << left << setw(25) << "-s,   --sudo"           << "Check for passwordless sudo access" << endl;
				cout << "  " << left << setw(25) << "-k,   --ssh-keys"       << "Scan for world-writable SSH keys" << endl;
				cout << "  " << left << setw(25) << "-b,   --suid"           << "Scan for SUID binaries" << endl;
				cout << "  " << left << setw(25) << "-a,   --all"		<< "Run all security audits" << endl;
				cout << "  " << left << setw(25) << "-d,   --directory <path>" << "Check directory for changes" << endl;
				cout << "  " << left << setw(25) << "-r,   --root"           << "Force scan starting at root" << endl;
				cout << "  " << left << setw(25) << "-w,   --write-new"      << "Ignore saved timestamps" << endl;
				cout << "  " << left << setw(25) << "-i,   --ignore-hidden"  << "Skip hidden files and folders" << endl;
				cout << "  " << left << setw(25) << "-g,   --sudo-group"     << "List users with sudo privileges" << endl;
				cout << "  " << left << setw(25) << "-U,   --system-update"  << "Check if system is up to date" << endl;

				return 0;
				break;
			//confusing about how this would work with all- because of how --all exists?
			case 'r':
				dirFlags.rootCheck = true;
				break;

			case 'w':
				dirFlags.writeNew = true;
				break;

			case 'i':
				dirFlags.ignoreHidden = true;
				break;
			

			case 'a': {
				//add all your functions here- this is the "all" option
				suid_binary_audit(logDir);
				world_writable_ssh_keys(logDir);
				passwordless_sudo_access(logDir);
				check_sources_list();
				
				vector<string> paths = get_paths();
				int problems = get_path_vulnerabilities(paths);
				cout << "PATH scan complete. " << problems << " potential issue(s) found. Issues outputted to PATH.txt\n";
			
				check_cron_jobs();
				
				//i'm choosing to not include directory case in this because it would require the --all flag to take an argument, which wouldn't really work if another one required an argument as well
				
				check_empty_passwords();
				
				check_ufw();
				
				check_sudo();
				check_sys_updated(); 
				return 0;
				break;
			}
			
			case 'g':  
				check_sudo();
				break;

			case 'U':  
				check_sys_updated();
				break;

			case 'u': {
				check_empty_passwords();
				break;
			}
			
			case 'x':
				check_sources_list();
				break;
			
			case 's':
				passwordless_sudo_access(logDir);
				break;
			
			case 'k':
				world_writable_ssh_keys(logDir);
				break;
			
			case 'b':
				suid_binary_audit(logDir);
				break;
			
			case 'd': {
				if (optarg == nullptr) {
					cerr << "Error: --directory requires an argument" << endl;
					return 1;
				}
				string dir = optarg;
				check_directory_for_changes(dir, dirFlags);
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
			case 'f': {
				check_ufw();
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
