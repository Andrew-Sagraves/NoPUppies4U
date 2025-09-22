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

    while (getline(file, line)) {
        // Only collect lines that contain "deb" (actual repository sources)
        if (line.find("deb") != string::npos) {
            sources.push_back(line);
        }
    }

    // Print out the unknown sources
    for (size_t i = 0; i < sources.size(); i++) {
        // Check if it's not one of the valid Ubuntu sources
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

    file.close();
    cout << "> Num Unknown sources: " << numUnknownSources << endl;
}
