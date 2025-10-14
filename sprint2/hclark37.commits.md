79ced8ce (hclark37 2025-09-22 14:19:52 -0500   2) #include <string.h>
79ced8ce (hclark37 2025-09-22 14:19:52 -0500   3) #include <stdio.h>
79552b84 (hclark37 2025-09-29 19:36:06 -0400   4) #include <string>
f0a75a81 (hclark37 2025-09-23 07:35:02 -0500   5) //#include "main.hpp"
f0a75a81 (hclark37 2025-09-23 07:35:02 -0500   6) //#include "asagrave.h"
79ced8ce (hclark37 2025-09-22 14:19:52 -0500   7) #include <getopt.h> 
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400   8) #include "asagrave.h"
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400   9) #include "bmulli21.h"
2a57cd07 (hclark37 2025-09-23 07:55:00 -0500  10) #include "jdong11.h"
79552b84 (hclark37 2025-09-29 19:36:06 -0400  11) #include "kbissonn.h"
4390cb40 (hclark37 2025-10-12 13:27:59 -0400  12) #include "hclark37.h"
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400  13) #include <vector>
bf29ce6c (hclark37 2025-09-29 09:09:26 -0400  14) #include <iomanip>
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  15) 
f0a75a81 (hclark37 2025-09-23 07:35:02 -0500  18) int main(int argc, char* argv[]) {
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  19) 	
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  20) 	/*
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  21) 		- Check to see if argv[i] is a valid --(command)
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  22) 		- If (command) requires input, check argv[i + 1] for valid input. If so, increment i to skip it for next, and process it, otherwise, error? 
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  23) 		- Goal: have it always either work or fail. No unexpected states 
b9d5ce62 (hclark37 2025-10-13 21:30:52 -0400  24) 
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  25) 		
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  26) 		-p for path
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  27) 		-c for crontab
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  28) 		-s for sudo
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  29) 		-a for all?
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  30) 		-v 
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  31) 	*/
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  32) 	
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  33) 	static struct option long_options[] = {
1917181d (hclark37 2025-10-13 21:30:26 -0400  34) 		{"help",          no_argument,       0, 'h'},
1917181d (hclark37 2025-10-13 21:30:26 -0400  35) 		{"crontab",       no_argument,       0, 'c'},
1917181d (hclark37 2025-10-13 21:30:26 -0400  36) 		{"path",          no_argument,       0, 'p'}, 
1917181d (hclark37 2025-10-13 21:30:26 -0400  37) 		{"sudo",          no_argument,       0, 's'}, 
1917181d (hclark37 2025-10-13 21:30:26 -0400  38) 		{"ssh-keys",      no_argument,       0, 'k'},
1917181d (hclark37 2025-10-13 21:30:26 -0400  39) 		{"suid",          no_argument,       0, 'b'},  
1917181d (hclark37 2025-10-13 21:30:26 -0400  40) 		{"all",           no_argument,       0, 'a'},  
1917181d (hclark37 2025-10-13 21:30:26 -0400  41) 		{"directory",     required_argument, 0, 'd'},
1917181d (hclark37 2025-10-13 21:30:26 -0400  42) 		{"root",          no_argument,       0, 'r'},
1917181d (hclark37 2025-10-13 21:30:26 -0400  43) 		{"write-new",     no_argument,       0, 'w'},
1917181d (hclark37 2025-10-13 21:30:26 -0400  44) 		{"ignore-hidden", no_argument,       0, 'i'},
1917181d (hclark37 2025-10-13 21:30:26 -0400  45) 		{"sudo-group",     no_argument, 0, 'g'}, 
1917181d (hclark37 2025-10-13 21:30:26 -0400  46) 		{"system-update",  no_argument, 0, 'U'},
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  47) 		{0, 0, 0, 0}
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  48) 	};
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  49) 	
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  50) 	int opt = 0;
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  51) 	int options_index = 0;
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  52) 	
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400  53) 	if (argc == 1) {
2a57cd07 (hclark37 2025-09-23 07:55:00 -0500  54) 		cout << "Usage: nopuppies4u [options]" << endl;
2a57cd07 (hclark37 2025-09-23 07:55:00 -0500  55) 		return 0;
2a57cd07 (hclark37 2025-09-23 07:55:00 -0500  56) 	}
2a57cd07 (hclark37 2025-09-23 07:55:00 -0500  57) 	
1917181d (hclark37 2025-10-13 21:30:26 -0400  58) 	DirectoryCheckFlags dirFlags;
1917181d (hclark37 2025-10-13 21:30:26 -0400  59) 	
1917181d (hclark37 2025-10-13 21:30:26 -0400  60) 	string logDir = "./"; //may not be hardcoded later
1917181d (hclark37 2025-10-13 21:30:26 -0400  61) 	
1917181d (hclark37 2025-10-13 21:30:26 -0400  62) 	while ((opt = getopt_long(argc, argv, "hrwicpksbagUd:", long_options, &options_index)) != -1) { //hvscp lets short options work, like -s
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400  63) 		//you have to make sure to add any additional options to that ""
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400  64) 		
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  65) 		switch (opt) {
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  66) 			case 'h':
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  67) 				cout << "Usage: nopuppies4u [options]" << endl;
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  68) 				cout << "Options:" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  69) 				cout << "  " << left << setw(25) << "-h,   --help"           << "Show this help message" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  70) 				cout << "  " << left << setw(25) << "-c,   --crontab"        << "Check crontab" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  71) 				cout << "  " << left << setw(25) << "-p,   --path"           << "Check PATH for vulnerabilities" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  72) 				cout << "  " << left << setw(25) << "-s,   --sudo"           << "Check for passwordless sudo access" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  73) 				cout << "  " << left << setw(25) << "-k,   --ssh-keys"       << "Scan for world-writable SSH keys" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  74) 				cout << "  " << left << setw(25) << "-b,   --suid"           << "Scan for SUID binaries" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  75) 				cout << "  " << left << setw(25) << "-a,   --all"		<< "Run all security audits" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  76) 				cout << "  " << left << setw(25) << "-d,   --directory <path>" << "Check directory for changes" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  77) 				cout << "  " << left << setw(25) << "-r,   --root"           << "Force scan starting at root" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  78) 				cout << "  " << left << setw(25) << "-w,   --write-new"      << "Ignore saved timestamps" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  79) 				cout << "  " << left << setw(25) << "-i,   --ignore-hidden"  << "Skip hidden files and folders" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  80) 				cout << "  " << left << setw(25) << "-g,   --sudo-group"     << "List users with sudo privileges" << endl;
1917181d (hclark37 2025-10-13 21:30:26 -0400  81) 				cout << "  " << left << setw(25) << "-U,   --system-update"  << "Check if system is up to date" << endl;
bf29ce6c (hclark37 2025-09-29 09:09:26 -0400  82) 
79ced8ce (hclark37 2025-09-22 14:19:52 -0500  83) 				return 0;
2a57cd07 (hclark37 2025-09-23 07:55:00 -0500  84) 				break;
1917181d (hclark37 2025-10-13 21:30:26 -0400  85) 			//confusing about how this would work with all- because of how --all exists?
1917181d (hclark37 2025-10-13 21:30:26 -0400  86) 			case 'r':
1917181d (hclark37 2025-10-13 21:30:26 -0400  87) 				dirFlags.rootCheck = true;
1917181d (hclark37 2025-10-13 21:30:26 -0400  88) 				break;
1917181d (hclark37 2025-10-13 21:30:26 -0400  89) 
1917181d (hclark37 2025-10-13 21:30:26 -0400  90) 			case 'w':
1917181d (hclark37 2025-10-13 21:30:26 -0400  91) 				dirFlags.writeNew = true;
1917181d (hclark37 2025-10-13 21:30:26 -0400  92) 				break;
1917181d (hclark37 2025-10-13 21:30:26 -0400  93) 
1917181d (hclark37 2025-10-13 21:30:26 -0400  94) 			case 'i':
1917181d (hclark37 2025-10-13 21:30:26 -0400  95) 				dirFlags.ignoreHidden = true;
1917181d (hclark37 2025-10-13 21:30:26 -0400  96) 				break;
1917181d (hclark37 2025-10-13 21:30:26 -0400  97) 			
1917181d (hclark37 2025-10-13 21:30:26 -0400  98) 
57a0e9e7 (hclark37 2025-09-29 09:13:05 -0400  99) 			case 'a': {
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 100) 				//add all your functions here- this is the "all" option
1917181d (hclark37 2025-10-13 21:30:26 -0400 101) 				suid_binary_audit(logDir);
1917181d (hclark37 2025-10-13 21:30:26 -0400 102) 				world_writable_ssh_keys(logDir);
1917181d (hclark37 2025-10-13 21:30:26 -0400 103) 				passwordless_sudo_access(logDir);
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 104) 				check_sources_list();
1917181d (hclark37 2025-10-13 21:30:26 -0400 105) 				
cd3592e8 (hclark37 2025-09-29 09:12:29 -0400 106) 				vector<string> paths = get_paths();
cd3592e8 (hclark37 2025-09-29 09:12:29 -0400 107) 				int problems = get_path_vulnerabilities(paths);
cd3592e8 (hclark37 2025-09-29 09:12:29 -0400 108) 				cout << "PATH scan complete. " << problems << " potential issue(s) found. Issues outputted to PATH.txt\n";
cd3592e8 (hclark37 2025-09-29 09:12:29 -0400 109) 			
79552b84 (hclark37 2025-09-29 19:36:06 -0400 110) 				check_cron_jobs();
79552b84 (hclark37 2025-09-29 19:36:06 -0400 111) 				
79552b84 (hclark37 2025-09-29 19:36:06 -0400 112) 				//i'm choosing to not include directory case in this because it would require the --all flag to take an argument, which wouldn't really work if another one required an argument as well
79552b84 (hclark37 2025-09-29 19:36:06 -0400 113) 				
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 114) 				check_empty_passwords();
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 115) 				
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 116) 				check_ufw();
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 117) 				
1917181d (hclark37 2025-10-13 21:30:26 -0400 118) 				check_sudo();
1917181d (hclark37 2025-10-13 21:30:26 -0400 119) 				check_sys_updated(); 
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 120) 				return 0;
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 121) 				break;
57a0e9e7 (hclark37 2025-09-29 09:13:05 -0400 122) 			}
1917181d (hclark37 2025-10-13 21:30:26 -0400 123) 			
1917181d (hclark37 2025-10-13 21:30:26 -0400 124) 			case 'g':  
1917181d (hclark37 2025-10-13 21:30:26 -0400 125) 				check_sudo();
1917181d (hclark37 2025-10-13 21:30:26 -0400 126) 				break;
1917181d (hclark37 2025-10-13 21:30:26 -0400 127) 
1917181d (hclark37 2025-10-13 21:30:26 -0400 128) 			case 'U':  
1917181d (hclark37 2025-10-13 21:30:26 -0400 129) 				check_sys_updated();
1917181d (hclark37 2025-10-13 21:30:26 -0400 130) 				break;
1917181d (hclark37 2025-10-13 21:30:26 -0400 131) 
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 132) 			case 'u': {
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 133) 				check_empty_passwords();
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 134) 				break;
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 135) 			}
1917181d (hclark37 2025-10-13 21:30:26 -0400 136) 			
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 137) 			case 'x':
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 138) 				check_sources_list();
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 139) 				break;
79552b84 (hclark37 2025-09-29 19:36:06 -0400 140) 			
1917181d (hclark37 2025-10-13 21:30:26 -0400 141) 			case 's':
1917181d (hclark37 2025-10-13 21:30:26 -0400 142) 				passwordless_sudo_access(logDir);
1917181d (hclark37 2025-10-13 21:30:26 -0400 143) 				break;
1917181d (hclark37 2025-10-13 21:30:26 -0400 144) 			
1917181d (hclark37 2025-10-13 21:30:26 -0400 145) 			case 'k':
1917181d (hclark37 2025-10-13 21:30:26 -0400 146) 				world_writable_ssh_keys(logDir);
1917181d (hclark37 2025-10-13 21:30:26 -0400 147) 				break;
1917181d (hclark37 2025-10-13 21:30:26 -0400 148) 			
1917181d (hclark37 2025-10-13 21:30:26 -0400 149) 			case 'b':
1917181d (hclark37 2025-10-13 21:30:26 -0400 150) 				suid_binary_audit(logDir);
1917181d (hclark37 2025-10-13 21:30:26 -0400 151) 				break;
1917181d (hclark37 2025-10-13 21:30:26 -0400 152) 			
79552b84 (hclark37 2025-09-29 19:36:06 -0400 153) 			case 'd': {
79552b84 (hclark37 2025-09-29 19:36:06 -0400 154) 				if (optarg == nullptr) {
79552b84 (hclark37 2025-09-29 19:36:06 -0400 155) 				cerr << "Error: --directory requires an argument" << endl;
79552b84 (hclark37 2025-09-29 19:36:06 -0400 156) 				return 1;
79552b84 (hclark37 2025-09-29 19:36:06 -0400 157) 				}
1917181d (hclark37 2025-10-13 21:30:26 -0400 158) 				string dir = optarg;
1917181d (hclark37 2025-10-13 21:30:26 -0400 159) 				check_directory_for_changes(dir, dirFlags);
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 160) 				break;
79552b84 (hclark37 2025-09-29 19:36:06 -0400 161) 			}
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 162) 			case 'c':
79552b84 (hclark37 2025-09-29 19:36:06 -0400 163) 				check_cron_jobs();
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 164) 				break;
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 165) 			case 'p': {
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 166) 				vector<string> paths = get_paths();
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 167) 				int problems = get_path_vulnerabilities(paths);
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 168) 				cout << "PATH scan complete. " << problems << " potential issue(s) found. Issues outputted to PATH.txt\n";
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 169) 				break;
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 170) 			}
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 171) 			case 'f': {
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 172) 				check_ufw();
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 173) 				break;
4390cb40 (hclark37 2025-10-12 13:27:59 -0400 174) 			}
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 175) 			case '?': 
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 176) 				//apparently occurs when it gets an unknown flag? 
8dae9d7c (hclark37 2025-09-26 17:55:33 -0400 177) 				cerr << "Error: invalid argument '" << argv[optind - 1] << "'\n";  
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 178) 			default:
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 179) 				return 1;
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 180) 		}
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 181) 	}
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 182) 	
79ced8ce (hclark37 2025-09-22 14:19:52 -0500 183) 	return 0;

e4b48043 (hclark37 2025-10-12 13:11:28 -0400  1) #include "hclark37.h"
e4b48043 (hclark37 2025-10-12 13:11:28 -0400  2) 
e4b48043 (hclark37 2025-10-12 13:11:28 -0400  3) void check_empty_passwords() {
e4b48043 (hclark37 2025-10-12 13:11:28 -0400  4)     ifstream shadow_file("/etc/shadow");
e4b48043 (hclark37 2025-10-12 13:11:28 -0400  5)     if (!shadow_file.is_open()) {
e4b48043 (hclark37 2025-10-12 13:11:28 -0400  6)         cerr << "Error: Unable to open /etc/shadow. Are you running as root?" << endl;
e4b48043 (hclark37 2025-10-12 13:11:28 -0400  7)         return;
e4b48043 (hclark37 2025-10-12 13:11:28 -0400  8)     }
e4b48043 (hclark37 2025-10-12 13:11:28 -0400  9) 
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 10)     string line;
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 11)     bool found_issue = false;
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 12) 
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 13)     cout << " - Checking for accounts with empty or no password..." << endl;
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 14) 
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 15)     while (getline(shadow_file, line)) {
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 16)         stringstream ss(line);
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 17)         string user_name, password_hash;
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 18) 
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 19)         // grab username and password hash; they are separated by ':'
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 20)         getline(ss, user_name, ':');
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 21)         getline(ss, password_hash, ':');
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 22) 	
2ea2b82a (hclark37 2025-10-12 13:16:02 -0400 23) 		//specifiers for no password
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 24)         if (password_hash.empty() || password_hash == "*" || password_hash == "!" || password_hash == "!!") {
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 25)             cout << "  - Insecure account found: " << user_name << endl;
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 26)             found_issue = true;
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 27)         }
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 28)     }
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 29) 
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 30)     if (found_issue == false) {
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 31)         cout << " - No accounts with empty/unsafe passwords found." << endl;
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 32)     }
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 33) 
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 34)     shadow_file.close();
e4b48043 (hclark37 2025-10-12 13:11:28 -0400 35) }

0c11e542 (hclark37 2025-10-12 13:11:50 -0400  1) #ifndef HCLARK37_H
0c11e542 (hclark37 2025-10-12 13:11:50 -0400  2) #define HCLARK37_H
0c11e542 (hclark37 2025-10-12 13:11:50 -0400  3) 
0c11e542 (hclark37 2025-10-12 13:11:50 -0400  4) #include <iostream>
0c11e542 (hclark37 2025-10-12 13:11:50 -0400  5) #include <fstream>
0c11e542 (hclark37 2025-10-12 13:11:50 -0400  6) #include <sstream>
0c11e542 (hclark37 2025-10-12 13:11:50 -0400  7) #include <string>
0c11e542 (hclark37 2025-10-12 13:11:50 -0400  8) #include <vector>
0c11e542 (hclark37 2025-10-12 13:11:50 -0400  9) 
0c11e542 (hclark37 2025-10-12 13:11:50 -0400 10) using namespace std;
0c11e542 (hclark37 2025-10-12 13:11:50 -0400 11) 
0c11e542 (hclark37 2025-10-12 13:11:50 -0400 12) void check_empty_passwords();
0c11e542 (hclark37 2025-10-12 13:11:50 -0400 13) 
0c11e542 (hclark37 2025-10-12 13:11:50 -0400 14) #endif

68389fb7 (hclark37 2025-10-12 15:31:49 -0400  1) #!/bin/bash
68389fb7 (hclark37 2025-10-12 15:31:49 -0400  2) #DO NOT RUN ON YOUR OWN MACHINE
68389fb7 (hclark37 2025-10-12 15:31:49 -0400  3) #INTENDED ONLY FOR TEST VIRTUAL MACHINES
68389fb7 (hclark37 2025-10-12 15:31:49 -0400  4) #WILL INTRODUCE DANGEROUS VULNERABILITIES INTO YOUR MACHINE!
68389fb7 (hclark37 2025-10-12 15:31:49 -0400  5) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400  6) echo "Creating PATH vulnerabilities..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400  7) mkdir -p /tmp/malicious_bin
68389fb7 (hclark37 2025-10-12 15:31:49 -0400  8) chmod 777 /tmp/malicious_bin
68389fb7 (hclark37 2025-10-12 15:31:49 -0400  9) echo '#!/bin/bash
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 10) echo "Malicious command"
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 11) exit 1' > /tmp/malicious_bin/ls
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 12) chmod +x /tmp/malicious_bin/ls
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 13) #add to front of path so it goes before your other commands
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 14) echo 'export PATH="/tmp/malicious_bin:$PATH"' >> /etc/environment
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 15) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 16) echo "Creating world-writable directory..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 17) mkdir -p /usr/local/insecure_bin
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 18) chmod 777 /usr/local/insecure_bin
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 19) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 20) #create insecure users
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 21) echo "Creating password vulnerabilities..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 22) useradd -m -s /bin/bash emptyuser
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 23) usermod -p '' emptyuser
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 24) useradd -m -s /bin/bash lockeduser
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 25) usermod -p '!' lockeduser
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 26) useradd -m -s /bin/bash nopassuser
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 27) usermod -p '*' nopassuser
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 28) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 29) echo "Adding fake package sources..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 30) echo "# FAKE SOURCE" >> /etc/apt/sources.list
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 31) echo "deb http://fakerepo.com/ubuntu/ focal main" >> /etc/apt/sources.list
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 32) #this is for newer ubuntu versions
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 33) echo "Types: deb
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 34) URIs: http://fakerepo.com/ubuntu/
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 35) Suites: focal
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 36) Components: main" > /etc/apt/sources.list.d/fakerepo.sources
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 37) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 38) echo "Adding users to sudo group..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 39) usermod -aG sudo emptyuser
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 40) usermod -aG sudo testuser
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 41) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 42) echo "Creating vulnerable cron jobs..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 43) echo '#!/bin/bash
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 44) echo "Vulnerable cron job"' > /etc/cron.hourly/insecure-cron
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 45) chmod 666 /etc/cron.hourly/insecure-cron
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 46) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 47) echo '#!/bin/bash
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 48) echo "Vulnerable script from cron"' > /tmp/cron-job.sh
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 49) chmod 666 /tmp/cron-job.sh
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 50) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 51) echo "Creating world-writable files..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 52) touch /usr/local/bin/world-writable
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 53) chmod 666 /usr/local/bin/world-writable
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 54) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 55) touch /bin/.hidden-vulnerable
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 56) chmod 666 /bin/.hidden-vulnerable
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 57) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 58) echo "Creating duplicate executables..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 59) cp /bin/ls /tmp/malicious_bin/ls-duplicate
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 60) cp /bin/cat /usr/local/insecure_bin/cat
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 61) cp /bin/echo /tmp/echo
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 62) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 63) echo "Disabling UFW..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 64) ufw disable 2>/dev/null || true
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 65) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 66) #not fully certain on how this works. found in research about how apt checks if packages are up to date 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 67) echo "Creating outdated packages..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 68) touch /var/lib/apt/periodic/update-success-stamp
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 69) find /var/lib/apt/lists -type f -delete 2>/dev/null || true
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 70) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 71) #using aforementioned created user 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 72) echo "Creating weak SSH keys..."
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 73) mkdir -p /home/emptyuser/.ssh
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 74) ssh-keygen -t rsa -b 1024 -f /home/emptyuser/.ssh/id_rsa -N "" -q
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 75) chmod 777 /home/emptyuser/.ssh
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 76) chmod 666 /home/emptyuser/.ssh/id_rsa
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 77) 
68389fb7 (hclark37 2025-10-12 15:31:49 -0400 78) echo "Vulnerability creation complete."

965b3957 (hclark37        2025-10-13 21:36:35 -0400 30) bool passwordless_sudo_access(const std::string& logDir);
965b3957 (hclark37        2025-10-13 21:36:35 -0400 31) bool world_writable_ssh_keys(const std::string& logDir);
965b3957 (hclark37        2025-10-13 21:36:35 -0400 32) bool suid_binary_audit(const std::string& logDir);
965b3957 (hclark37        2025-10-13 21:36:35 -0400 33) 

3d6cffa6 (hclark37 2025-10-12 13:45:32 -0400  1) nopuppies4u: main.o asagrave.o bmulli21.o jdong11.o kbissonn.o hclark37.o
3d6cffa6 (hclark37 2025-10-12 13:45:32 -0400  2) 	g++ -o nopuppies4u main.o asagrave.o bmulli21.o jdong11.o kbissonn.o hclark37.o
a121caec (hclark37 2025-09-26 17:58:29 -0400  3) 
a121caec (hclark37 2025-09-26 17:58:29 -0400  4) main.o: main.cpp asagrave.h bmulli21.h jdong11.h
a121caec (hclark37 2025-09-26 17:58:29 -0400  5) 	g++ -c main.cpp
a121caec (hclark37 2025-09-26 17:58:29 -0400  6) 
a121caec (hclark37 2025-09-26 17:58:29 -0400  7) asagrave.o: asagrave.cpp asagrave.h
a121caec (hclark37 2025-09-26 17:58:29 -0400  8) 	g++ -c asagrave.cpp
a121caec (hclark37 2025-09-26 17:58:29 -0400  9) 
3d6cffa6 (hclark37 2025-10-12 13:45:32 -0400 10) hclark37.o: hclark37.cpp hclark37.h
3d6cffa6 (hclark37 2025-10-12 13:45:32 -0400 11) 	g++ -c hclark37.cpp
3d6cffa6 (hclark37 2025-10-12 13:45:32 -0400 12) 
a121caec (hclark37 2025-09-26 17:58:29 -0400 13) bmulli21.o: bmulli21.cpp bmulli21.h
a121caec (hclark37 2025-09-26 17:58:29 -0400 14) 	g++ -c bmulli21.cpp
a121caec (hclark37 2025-09-26 17:58:29 -0400 15) 
a121caec (hclark37 2025-09-26 17:58:29 -0400 16) jdong11.o: jdong11.cpp jdong11.h
a121caec (hclark37 2025-09-26 17:58:29 -0400 17) 	g++ -c jdong11.cpp
a121caec (hclark37 2025-09-26 17:58:29 -0400 18) 
dbb38a17 (hclark37 2025-09-29 19:37:27 -0400 19) kbissonn.o: kbissonn.cpp kbissonn.h
dbb38a17 (hclark37 2025-09-29 19:37:27 -0400 20) 	g++ -c kbissonn.cpp
dbb38a17 (hclark37 2025-09-29 19:37:27 -0400 21) 
26c23adb (hclark37 2025-09-23 07:52:37 -0500 22) clean:
a121caec (hclark37 2025-09-26 17:58:29 -0400 23) 	rm -f *.o program
