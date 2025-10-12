#include "hclark37.h"

void check_empty_passwords() {
    ifstream shadow_file("/etc/shadow");
    if (!shadow_file.is_open()) {
        cerr << "Error: Unable to open /etc/shadow. Are you running as root?" << endl;
        return;
    }

    string line;
    bool found_issue = false;

    cout << " - Checking for accounts with empty or no password..." << endl;

    while (getline(shadow_file, line)) {
        stringstream ss(line);
        string user_name, password_hash;

        // grab username and password hash; they are separated by ':'
        getline(ss, user_name, ':');
        getline(ss, password_hash, ':');
	
	//specifiers for no password
        if (password_hash.empty() || password_hash == "*" || password_hash == "!" || password_hash == "!!") {
            cout << "  - Insecure account found: " << user_name << endl;
            found_issue = true;
        }
    }

    if (found_issue == false) {
        cout << " - No accounts with empty/unsafe passwords found." << endl;
    }

    shadow_file.close();
}
