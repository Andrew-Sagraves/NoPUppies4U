#include "jdong11.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <limits>

using namespace std;

// checks /etc/apt/sources.list if Ubuntu <22.04
// checks /etc/apt/sources.list.d/ubuntu.sources if Ubuntu >=22.04

/*
    Area of improvement: Allow user to add their own known list of URLs
*/

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

    // checking for urls in sources.list and push into vector
    while (getline(file, line)) {
        if (line.find("http") != string::npos) {
            if (line[0] == '#') {   // skip commented lines
                continue; 
            }
            sources.push_back(line);
        }
    }

    // parse the /etc/apt/sources.list file again line by line for deb822 which means the actual sources are in a different path Ubuntu 22.04+
    file.clear();
    file.seekg(0, ios::beg);
    while (getline(file, line)) {
        if (line.find("deb822") != string::npos) {
            cout << "Found deb822 format, switching to ubuntu.sources file..." << endl;
            file.clear(); // clear any error flags
            file.close();
            file.open("/etc/apt/sources.list.d/ubuntu.sources");
            file.seekg(0, ios::beg); // reset file pointer to beginning
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
    // first check for user permissions: require root permission
    if (geteuid() != 0) {
        cerr << "You need \"root\" permission to check /etc/apt/sources.list" << endl;
        return;
    }

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

// Check system if it is up to date
void check_sys_updated() {
    // first check for user permissions: require root permission
    if (geteuid() != 0) {
        cerr << "You need \"root\" permission to check /etc/apt/sources.list" << endl;
        return;
    }

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
            cout << "!!! System is up to date ♡⸜(˶˃ ᵕ ˂˶)⸝♡" << endl << endl;
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
                cerr << "Invalid input. Please enter \'y\', \'n\', or \'l\'. (╯°□°）╯︵ ~┻━┻" << endl;
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

    cout << "All logs are in /var/log/NoPUppies4U/ ദ്ദി(｡•̀ ,<)~✩‧₊" << endl << endl;

    file.close();
}

// This function is a helper function for ufw_check().
// This function prints out instructions on how to use ufw_check().
void ufwHelpPrint() {
    cout << endl;
    cout << "Add Rule Main Menu Usage: " << endl;
    cout << "\'?\'      - help" << endl;
    cout << "\'l\'      - list current rules" << endl;
    cout << "\'b\'      - go back to menu" << endl;
    cout << "\'r\'      - reset template to default" << endl;
    cout << "\'p\'      - print current template" << endl;
    cout << "\'w\'      - enter Write Mode on template" << endl;
    cout << "\'a\'      - add rule based on current template" << endl << endl;

    cout << "Write Mode Usage: " << endl
    << "To add rule, fill in the template (6 total to fill)" << endl
    << "template: [allow|deny|reject|limit] [in|out] from [source] to [destination] [port#] proto [tcp|udp]" << endl
    << "Use \'any\' for \'source\' or \'destination\' for any ip address." << endl << endl

    << "Inside template writing mode (w)"
    << "a - allow:          allow traffic though firewall" << endl
    << "d - deny:           deny traffic through firewall (silently drop)" << endl
    << "r - reject:         deny packets though firewall (send back RST)" << endl
    << "c - limit:          allows traffic with rate-limit connections (cap)" << endl
    << "i - in:             inbound traffic" << endl
    << "o - out:            outbound traffic" << endl
    << "s [IP] - source [IP]:         source ip (origin of traffic)" << endl
    << "d [IP]- destination [IP]:    destination ip (where packets end up)" << endl
    << "p [1-65535] - port [1-65535]:           destination port" << endl
    << "f [tcp/udp]- proto[tcp/ufp]:          protocol of traffic (format)" << endl << endl

    << "Example Usage of Write Mode: " << endl
    << "1.) allow in from any to any 22 proto tcp: this allows ssh in"
    << "(any host on the internet can connect to this machine via ssh)" << endl
    << "2.) deny out from any to 8.8.8.8 port 53 proto udp: prevent host from"
    << "sending DNS queries to 8.8.8.8" << endl;
}

void ufwWriteTemplMode() {
    cout << "template mode" << endl;
}

// check the Uncomplicated Firewall status
void check_ufw() {
    // first check for user permissions: require root permission
    if (geteuid() != 0) {
        cerr << "You need \"root\" permission to check /etc/apt/sources.list" << endl;
        return;
    }

    system("apt list --installed | grep ufw > ufwtmpFile.txt");

    ifstream file("ufwtmpFile.txt");
    if (file.fail()) {
        cerr << "File failed" << endl << endl;
        file.close();
        return;
    }

    string line;
    string status;
    string input;
    bool continueSig = false;

    cout << "Checking firewall (UFW - Uncomplicated Firewall)..." << endl;

    // Initialization for ufw
    getline(file, line);
    if (line.find("ufw") != string::npos) {
        cout << "UFW installation found. - _ -" << endl;
    }
    else {
        system("apt --quiet --assume-yes install ufw");
    }
    file.clear();
    file.close();

    system("ufw status");
    cout << "? - for help" << endl << endl;

    while (!continueSig) {
        cout << "Main Menu(ufw)> ";
        cin >> status;

        ostringstream os;

        switch (status[0]) {
            case '?': 
                cout << "Enter \'y\' to enable, " << endl;
                cout << "enter \'n\' to disable, " << endl;
                cout << "enter \'r\' to reset," << endl;
                cout << "enter \'l\' to list rules, " << endl;
                cout << "enter \'a\' to add rule, " << endl;
                cout << "enter \'d\' to delete rule, " << endl;
                cout << "enter \'c\' to continue." << endl;
                break;
            case 'y':
                system("ufw enable");
                break;
            case 'n':
                system("ufw disable");
                break;
            case 'r': 
                system("ufw reset");
                cout << "ufw disabled" << endl;
                break;
            case 'l': 
                cout << "Current list of Firewall Rules (empty of no rules): " << endl;
                system("ufw status numbered");
                break;
            case 'a': {
                string command;
                string filter = "allow";
                string trafDir = "in";
                string source = "any";
                string dest = "any";
                string port = "22";
                string proto = "tcp";
                bool back = false;

                os.str("");
                os.clear();
                
                cout << "Add rules mode:" << endl << endl;
                cout << "To return back to menu enter \'b\'." << endl;
                cout << "Enter ? for help" << endl;
                cout << "Enter \'reset\' for any time to reset rules to default." << endl;

                while (!back) {
                    cout << "Add Rule Mode(ufw)> ";
                    cin >> input;
                    switch (input[0]) {
                        case '?': 
                            ufwHelpPrint();
                            break;
                        case 'l': 
                            system("ufw status numbered");
                            cout << "Current list of Firewall Rules (empty of no rules): " << endl;
                            break;
                        case 'b': 
                            cout << "Going back to ufw Main Menu" << endl << endl;
                            back = true;
                            break;
                        case 'r':
                            cout << "Template Reset" << endl;
                            filter = "";
                            trafDir = "";
                            source = "any";
                            dest = "any";
                            port = "";
                            proto = "tcp";
                            break;
                        case 'p':
                            cout << "Filter: " << filter << endl;
                            cout << "Traffic Direction: " << trafDir << endl;
                            cout << "Source IP: " << source << endl;
                            cout << "Destination IP: " << dest << endl;
                            cout << "Destination port: " << port << endl;
                            cout << "Protocol: " << proto << endl << endl;
                            break;
                        case 'w': 
                            ufwWriteTemplMode();
                            break;
                        case 'a': 
                            cout << "Adding Rule..." << endl;
                            os << "ufw " << filter << " " << trafDir << " from " << source << " to " << dest << " port "
                            << port << " proto " << proto;
                            command = os.str();

                            system(command.c_str());
                            break;
                        default: 
                            cout << endl;
                            cout << "Invalid Input:" << endl
                            << "? - help" << endl
                            << "l - list current rules" << endl
                            << "b - back to menu" << endl
                            << "r - reset template to default" << endl
                            << "p - print template" << endl
                            << "w - enter template write mode" << endl
                            << "a - adding rule to ufw" << endl << endl;
                            break;
                    }
                }
                break;
            }
            case 'd': {
                int num;
                bool backd = false;          // back to menu
                
                while (!backd) {
                    cout << "Current list of Firewall Rules (empty of no rules): " << endl;
                    system("ufw status numbered");
                    cout << "To return back to menu, enter \'0\'." << endl;
                    cout << "Delete rule with the associated number: ";
                    cin >> num;
                    cout << endl;

                    // error checking for ints
                    if (cin.fail()) {
                        cout << "Not an Integer." << endl;
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    else {
                        // user enter 0 return back to menu
                        if (num == 0) {
                            backd = true;
                            continue;
                        }
                        // delete the associated num firewall
                        string command;
                        os.str("");
                        os.clear();
                        os << "ufw delete " << num;
                        command = os.str();

                        system(command.c_str());
                        cout << endl;
                    }
                }
                break;
            }
            case 'c': 
                continueSig = true;
                break;
            default:
                cout << endl;
                cout << "Invalid Input: " << endl;
                cout << "Enter \'y\' to enable, " << endl;
                cout << "enter \'n\' to disable, " << endl;
                cout << "enter \'r\' to reset," << endl;
                cout << "enter \'l\' to list rules, " << endl;
                cout << "enter \'a\' to add rule, " << endl;
                cout << "enter \'d\' to delete rule, " << endl;
                cout << "enter \'c\' to continue." << endl;
                break;
        }

    }

    cout << endl << endl;

    system("rm ufwtmpFile.txt");
}