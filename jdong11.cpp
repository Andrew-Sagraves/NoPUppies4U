#include "jdong11.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <sstream>

using namespace std;

// runs /etc/apt/sources.list if Ubuntu <22.04
// runs /etc/apt/sources.list.d/ubuntu.sources if Ubuntu >=22.04
void check_sources_list() {
    // first check for user permissions: require root permission
    if (geteuid() != 0) {
        cerr << "You need \"root\" permission to check /etc/apt/sources.list" << endl;
        return;
    }

    // open file and error checking the file
    ifstream file("/etc/apt/sources.list");
    if (file.fail()) {
        cerr << "/etc/apt/sources.list failed to open." << endl;
        file.close();
        return;
    }
    
    // reading from file and checking if valid
    printf("Checking /etc/apt/sources.list...\n");
    
    string line; 
    int numUnknownSources = 0;
    vector<string> sources;
    bool deb822Format = false;

    // checking for urls in sources.list
    while (getline(file, line)) {
        if (line.find("http") != string::npos) {
            if (line[0] == '#') {   // skip commented lines
                continue; 
            }
            sources.push_back(line);
        }
    }

    file.clear(); // clear any error flags
    file.seekg(0, ios::beg); // reset file pointer to beginning

    // parse the file line by line for deb822 which means the actual sources are in a different path Ubuntu 22.04+
    while (getline(file, line)) {
        if (line.find("deb822") != string::npos) {
            cout << "Found deb822 format, switching to ubuntu.sources file..." << endl;
            file.close();
            file.clear();
            file.open("/etc/apt/sources.list.d/ubuntu.sources");
            if (file.fail()) {
                cerr << "/etc/apt/sources.list.d/ubuntu.sources failed to open." << endl;
                return;
            }
            deb822Format = true;
            break; 
        }
    }

    // If switched to deb822 format, check new file for URIs
    if (deb822Format) {
        cout << "Checking /etc/apt/sources.list.d/ubuntu.sources file..." << endl;
        while (getline(file, line)) {
            if (line.find("http") != string::npos) {
                if (line[0] == '#') {
                    continue;
                }
                sources.push_back(line);
            }
        }
    }

    // Print out the unknown sources
    cout << sources.size() << " total sources found..." << endl;
    for (size_t i = 0; i < sources.size(); i++) {
        if (!deb822Format) {
            // Check if it's not one of the valid Ubuntu sources with sources.list format
            if (
                sources[i].find("deb http://archive.ubuntu.com/ubuntu/") == string::npos &&
                sources[i].find("deb http://security.ubuntu.com/ubuntu/") == string::npos &&
                sources[i].find("deb-src http://archive.ubuntu.com/ubuntu/") == string::npos && 
                sources[i].find("deb-src http://security.ubuntu.com/ubuntu/") == string::npos &&
                sources[i].find("deb http://archive.canonical.com/ubuntu") == string::npos &&
                sources[i].find("deb-src http://archive.canonical.com/ubuntu") == string::npos
                ) {
                cout << "Unknown source found: " << sources[i] << endl;
                numUnknownSources += 1;
            }
        }
        // Check if it's not one of the valid Ubuntu sources with deb822 format
        else {
            if (
                sources[i].find("http://archive.ubuntu.com/ubuntu/") == string::npos &&
                sources[i].find("http://security.ubuntu.com/ubuntu/") == string::npos &&
                sources[i].find("http://archive.canonical.com/ubuntu") == string::npos &&
                sources[i].find("http://help.ubuntu.com/community/UpgradeNotes") == string::npos
                ) {
                cout << "Unknown source found: " << sources[i] << endl;
                numUnknownSources += 1;
            }
        }
    }

    file.close();
    cout << "!!! Num Unknown sources: " << numUnknownSources << " ^^^" << endl << endl;
}

// checks /etc/group for users in sudo group
void check_sudo() {
    // Open /etc/group file to check sudo group members
    ifstream file("/etc/group");
    if (file.fail()) {
        cerr << "/etc/group failed to open." << endl;
        return;
    }

    string line;
    stringstream sin;
    string user;
    bool comma = true;
    
    printf("Checking /etc/group for sudo users...\n");

    // reads each line in file
    while (getline(file, line)) {
        if (line.find("sudo:") != string::npos) {
            // gets skips sudo:x:27:
            line = line.substr(line.find(":") + 1); 
            line = line.substr(line.find(":") + 1);
            line = line.substr(line.find(":") + 1);
            
            // put line into stringstream for parsing
            sin.str(line);
            sin.clear();
            
            // parsing the comma-separated list of users into terminal
            cout << "!!! Users in sudo group: ";
            while (getline(sin, user, ',')) {
                if (comma) {
                    cout << user;
                    comma = false;
                }
                else {
                    cout << ", " << user;
                }
            }
            cout << endl << endl;
        }
    }
    file.close();    
}

void check_sys_updated() {
    printf("Checking if system is up to date...\n");
    system("mkdir -p /var/log/NoPUppies4U");
    system("apt update --quiet --assume-yes >/var/log/NoPUppies4U/update.log");
    system("apt list --upgradable > /var/log/NoPUppies4U/upgradable.log");

    string buffer;
    string userInput;
    bool updateSignal = false;      // true if system is not up to date
    bool updated = false;           // true if user chose to update system
    ifstream file("/var/log/NoPUppies4U/update.log");

    //error checking file
    if (file.fail()) {
        cerr << "/var/log/NoPUppies4U/update.log failed to open." << endl;
        return;
    }
    
    // parse through each line of update.log
    while (getline(file, buffer)) {
        if (buffer.find("All packages are up to date.") != string::npos) {
            cout << "!!! System is up to date (^_^)" << endl << endl;
            file.close();
            return;
        }
        else if (buffer.find("packages can be upgraded.") != string::npos || buffer.find("package can be upgraded.") != string::npos) {
            cout << "System is >NOT< up to date." << endl;
            updateSignal = true;
        }
    }

    // if system is not up to date, prompt user to upgrade now, skip upgrade, or list upgradable packages
    while (updateSignal) {
        userInput.clear();
        cout << "Enter \'n\' to skip upgrade. Enter \'y\' to upgrade now. Enter \'l\' to list packages upgradable list: > ";
        cin >> userInput;

        switch(userInput[0]) {
            case 'y': 
                cout << "Upgrading system now..." << endl;
                system("apt upgrade --quiet --assume-yes >/var/log/NoPUppies4U/upgrade.log 2>/var/log/NoPUppies4U/error.log");
                cout << "System upgrade complete. Check /var/log/NoPUppies4U/upgrade.log for details." << endl;
                updated = true;
                updateSignal = false;
                break;
            case 'n': 
                updateSignal = false;
                break;
            case 'l':
                system("apt list --upgradable");
                break;
            default: 
                cerr << "Invalid input. Please enter \'y\', \'n\', or \'l\'." << endl;
                break;
        }
    }

    // checking for errors in error.log if system was updated
    if (updated) {
        ifstream errorFile("/var/log/NoPUppies4U/error.log");

        // error checking error.log file
        if (errorFile.fail()) {
            cerr << "/var/log/NoPUppies4U/error.log failed to open." << endl;
            return;
        }
        if (errorFile.peek() == ifstream::traits_type::eof() ) {
            cout << "No errors found..." << endl;
        }
        else {
            cout << "Errors occurred during update. Check /var/log/NoPUppies4U/error.log for details." << endl;
        }
        errorFile.close();
    }

    cout << "All logs are in /var/log/NoPUppies4U/" << endl << endl;

    file.close();
}