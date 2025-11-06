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
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

bool VERBOSE = false;

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
		{"sudo",          optional_argument,       0, 's'}, 
		{"ssh-keys",      optional_argument,       0, 'k'},
		{"suid",          optional_argument,       0, 'b'},  
		{"all",           no_argument,       0, 'a'},  
		{"directory",     required_argument, 0, 'd'},
		{"root",          no_argument,       0, 'r'},
		{"write-new",     no_argument,       0, 'w'},
		{"ignore-hidden", no_argument,       0, 'i'},
		{"sudo-group",     no_argument, 0, 'g'}, 
		{"system-update",  no_argument, 0, 'U'},
		{"system-logs",  required_argument, 0, 'L'}, 
		{"sudoers", no_argument, 0, 'S'},
		{"log-dir", required_argument, 0, 'o'},
		{"verbose",  no_argument, 0, 'v'},
		{"ncat-scan",     no_argument,       0, 'N'}, 
		{"systemd-audit", no_argument, 0, 'D'},
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
	
	while ((opt = getopt_long(argc, argv, "hrwicpSagUd:L:ovk::s::b::ND", long_options, &options_index)) != -1) {
	//you have to make sure to add any additional options to that ""
		
		switch (opt) {
			
			case 'h':
				cout << "Usage: nopuppies4u [options]" << endl;
				cout << "Options:" << endl;
				cout << "  " << left << setw(25) << "-h,   --help"           << "Show this help message" << endl;
				cout << "  " << left << setw(25) << "-c,   --crontab"        << "Check crontab" << endl;
				cout << "  " << left << setw(25) << "-p,   --path"           << "Check PATH for vulnerabilities" << endl;
				cout << "  " << left << setw(25) << "-s,   --sudo [path]"           << "Check for passwordless sudo access" << endl;
				cout << "  " << left << setw(25) << "-k,   --ssh-keys [path]"       << "Scan for world-writable SSH keys" << endl;
				cout << "  " << left << setw(25) << "-b,   --suid [path]"           << "Scan for SUID binaries" << endl;
				cout << "  " << left << setw(25) << "-a,   --all"		<< "Run all security audits" << endl;
				cout << "  " << left << setw(25) << "-d,   --directory [path]" << "Check directory for changes" << endl;
				cout << "  " << left << setw(25) << "-r,   --root"           << "Force scan starting at root" << endl;
				cout << "  " << left << setw(25) << "-w,   --write-new"      << "Ignore saved timestamps" << endl;
				cout << "  " << left << setw(25) << "-i,   --ignore-hidden"  << "Skip hidden files and folders" << endl;
				cout << "  " << left << setw(25) << "-g,   --sudo-group"     << "List users with sudo privileges" << endl;
				cout << "  " << left << setw(25) << "-U,   --system-update"  << "Check if system is up to date" << endl;
				cout << "  " << left << setw(25) << "-L, --parse-logs <word1,word2>" << "Parse logs for keywords" << endl;
				cout << "  " << left << setw(25) << "-S,   --sudoers" << "Scan sudoers files for users with sudo access" << endl;
				cout << "  " << left << setw(25) << "-N,   --ncat-scan"      << "Scan for active reverse shells" << endl;
				cout << "  " << left << setw(25) << "-o,   --log-dir <path>" << "Specify output directory for logs" << endl;
				cout << "  " << left << setw(25) << "-v,   --verbose"        << "Enable verbose output (more detailed logs)" << endl;
				cout << "  " << left << setw(25) << "-D,   --systemd-audit"  << "Audit systemd files for ownership" << endl;

				return 0;
				break;
			case 'v':
				VERBOSE = true;
				cout << "Verbose mode enabled" << endl;
				break;
			
			case 'o': {
				if (optarg == nullptr) {
					cerr << "Error: --log-dir requires an argument" << endl;
					return 1;
				}
				
				logDir = optarg;
				
				if (!logDir.empty() && logDir[logDir.size() - 1] != '/') {
					logDir += '/'; 
				}
				
				struct stat st;
				if (stat(logDir.c_str(), &st) != 0) {
					cout << "Log directory does not exist. Creating: " << logDir << endl;

					if (mkdir(logDir.c_str(), 0755) != 0) {
						perror("Error creating directory");
						return 1;
					}
				} else if (!S_ISDIR(st.st_mode)) {
					cerr << "Error: Provided path exists but is not a directory: " << logDir << endl;
					return 1;
				}
				
				if (access(logDir.c_str(), W_OK) != 0) {
					cerr << "Error: No write permission for directory: " << logDir << endl;
					return 1;
				}	
				
				cout << "Log directory set to: " << logDir << endl;
				
				break;
			}

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
				ncat_backdoor();
				
				check_cron_jobs_verbose();
				
				check_sudoers_permissions();
				
				//i'm choosing to not include directory case in this because it would require the --all flag to take an argument, which wouldn't really work if another one required an argument as well
				
				
				check_ufw();
				
				check_sudo();
				check_sys_updated(); 
				return 0;
				break;
			}
			
			case 'D': {
				string filename = logDir + "systemd_audit.log";
				cout << "Running systemd unit audit, output: " << filename << endl;
				bool found = systemd_unit_audit(filename);
				if (found) {
					cout << "Potential issues discovered during systemd audit. See " << filename << endl;
				} else {
					cout << "No issues detected by systemd audit." << endl;
				}
				break;
			}
			
			case 'L': { 
				string argument = optarg; 
				vector<string> keywords;
				stringstream ss(argument);
				string word;
				while (getline(ss, word, ',')) {
					keywords.push_back(word);
				}
				parse_all_logs(keywords, logDir + "kernel_all_logs_report.txt");
				break;
			}
			case 'N':
				ncat_backdoor();
				break;


			case 'S':
				cout << "Checking sudoers files for explicit sudo users..." << endl;
				check_sudoers();
				break;
			
			case 'g':  
				check_sudo();
				break;

			case 'U':  
				check_sys_updated();
				break;

			case 'x':
				check_sources_list();
				break;
			
			case 'b': {
				string filename;

				if (optarg != nullptr) {
					filename = optarg;
				} else {
					filename = "suid_default.log";
				}

				suid_binary_audit(logDir + filename);
				break;
			}


			case 'k': {
				string filename;

				if (optarg != nullptr) {
					filename = optarg;
				} else {
					filename = "ssh_keys.log";
				}

				world_writable_ssh_keys(logDir + filename);
				break;
			}


			case 's': {
				string filename;

				if (optarg != nullptr) {
					filename = optarg;
				} else {
					filename = "sudo_audit.log";
				}

				passwordless_sudo_access(logDir + filename);
				break;
			}
			
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
				check_cron_jobs_verbose();
				break;
			//MAKE FUNCTIONALITY FOR CHOOSING LOCATION
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
				cerr << "Error: invalid argument \'" << argv[optind - 1] << "\'\n"; 
				break; 
			default:
				return 1;
		}
	}
	
	return 0;
}
