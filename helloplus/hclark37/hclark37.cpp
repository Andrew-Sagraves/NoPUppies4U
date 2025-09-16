#include <iostream>
//lets you read /etc/passwd 
#include <pwd.h>
//reads /etc/shadow which is like the encrypted password file 
#include <shadow.h>
#include <cstring> 
//for geteuid()
#include <unistd.h>


using namespace std;

int main() {
	//root execution check for /etc/shadow 
	if (geteuid() != 0) {
		cerr << "Error: This program must be run as root." << endl;
		return 1;
	}
	
	setpwent();  //starts reading at start of the password database 
	
	struct passwd *pw;
	/*
	char *pw_name: The user's login name.
	uid_t pw_uid: The numerical user ID (UID).
	gid_t pw_gid: The numerical group ID (GID) of the user's primary group.
	char *pw_dir: The path to the user's home directory.
	char *pw_shell: The path to the user's default login shell.
	*/
	
	bool fail_switch = false;
	int total_checked = 0;
	
	while ((pw = getpwent()) != nullptr) {
		//skips accounts like "daemon" which arent 0 and are less than 1000, which arent real logins
		if (pw->pw_uid < 1000 && pw->pw_uid != 0) {
			continue; 
		}
		//iterate the counter
		total_checked += 1;
		
		struct spwd *sp = getspnam(pw->pw_name);
		
		/*
		https://man7.org/linux/man-pages/man3/getspnam.3.html
		returns a pointer to a structure
		containing the broken-out fields of the record in the shadow
		password database that matches the username name.
		*/
		if (sp && sp->sp_pwdp && strlen(sp->sp_pwdp) == 0) {
			fail_switch = true;
			cout << pw->pw_name << " has no password!" << endl;
			if (pw->pw_uid == 0) {
				cout << "Warning: this user has ROOT permissions (UID 0)!" << endl;
			}
		}
	}
	if (fail_switch == false) {
		cout << "No users found without passwords." << endl;
	}
	cout << "Total users checked: " << total_checked << endl;
	
	endpwent();
	
	return 0;
}
