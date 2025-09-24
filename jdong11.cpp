#include "jdong11.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <sstream>

using namespace std;

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

    // parse the file line by line for deb822 which means the actual sources are in a different path
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
    cout << sources.size() << " sources found..." << endl;
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
