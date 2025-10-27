#include "bmulli21.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include <map>
#include <sstream>

using namespace std;

extern bool VERBOSE;

//Loads previous date modified time from a file into a map
map<string, time_t> load_previous_date_modified(const string& dateModifiedFile) {
    map<string, time_t> timestamps;
    ifstream recordFile(dateModifiedFile);

    if (!recordFile.is_open()) {
        cout << "date_modified.txt not found. Creating new file." << endl;
        return timestamps; //Return an empty map
    }

    string line;

    while (getline(recordFile, line)) {
        stringstream stringStream(line);
        string filePath;
        time_t dateModified;
        
        //Format = <filepath> - <datemodified>
        if (getline(stringStream, filePath, '-') && stringStream >> dateModified) {
            //Removes space at end of filePath if it exists
            if (!filePath.empty() && filePath.back() == ' ') {
                filePath.pop_back();
            }

            timestamps[filePath] = dateModified;
        }
    }

    recordFile.close();

    return timestamps;
}


//Creates a vector of all files in a directory and subdirectories
vector<string> get_all_files_recursively(const string& directoryPath) {
    vector<string> filePath;

    if (!filesystem::is_directory(directoryPath)) {
        cerr << "Error: Not a valid directory - " << directoryPath << endl;
        return filePath; // Return empty list
    }

    //Iterators to go through directory and subdirectories
    filesystem::recursive_directory_iterator iterator(directoryPath, filesystem::directory_options::skip_permission_denied);
    filesystem::recursive_directory_iterator endingIterator;

    for (; iterator != endingIterator; ++iterator) {
        try {
            const auto& path = iterator->path();
            string pathStr = path.string();
            
            // Skip hidden directories/files if ignoreHidden flag is set
            if (pathStr.find("/.")  != string::npos || pathStr.find("\\.") != string::npos) {
                iterator.disable_recursion_pending();
                continue;
            }

            //Only adds files, not directories or other types like symlinks
            if (filesystem::is_regular_file(path)) {
                filePath.push_back(pathStr);
            }
        } catch (const filesystem::filesystem_error& e) {
            cerr << "Error accessing path: " << e.what() << endl;
            continue;
        }
    }
    
    return filePath;
    
    return filePath;
}

//This is an all in one function that does everything.
//Checks a directory for changed files, generates a report, and updates records.
void check_directory_for_changes(const string& checkingDirectory, const DirectoryCheckFlags& flags) {
    // If root check flag is set and directory isn't already root, start from root
    string effectiveDirectory = checkingDirectory;
    if (flags.rootCheck && checkingDirectory != "/") {
        effectiveDirectory = "/";
        cout << "Root check flag set. Checking entire system from root directory." << endl;
    }

    const string dateModifiedTxt = "date_modified.txt";
    const string newlyDateModifiedTxt = "new_date_modified.txt";

    //Load the old timestamps into our efficient map, unless writeNew flag is set
    map<string, time_t> oldDateModified;
    if (!flags.writeNew) {
        oldDateModified = load_previous_date_modified(dateModifiedTxt);
    } else {
        cout << "Write new flag set. Ignoring existing date_modified.txt file." << endl;
    }

    //Get a list of all files currently in the directory and subdirectories
    vector<string> currentFiles = get_all_files_recursively(effectiveDirectory);

    if (currentFiles.empty()) {
        cout << "No files found in directory." << endl;
        return;
    }

    vector<string> newlyModifiedFiles;

    //New file to store updated timestamps
    ofstream newModifiedFile(dateModifiedTxt, ios::trunc);

    if (!newModifiedFile.is_open()) {
        cerr << "Error: Could not open record file - " << dateModifiedTxt << endl;
        return;
    }

    cout << "Scanning " << currentFiles.size() << " files..." << endl;

    //Loops through all current files to check for modifications
    for (size_t i = 0; i < currentFiles.size(); ++i) {
        const string& filePath = currentFiles[i];
        struct stat fileStat;

        //Gets the file's current modified time
        if (stat(filePath.c_str(), &fileStat) == 0) {
            time_t currentModifiedTime = fileStat.st_mtime;

            //Format: <filepath> - <datemodified>
            newModifiedFile << filePath << " - " << currentModifiedTime << "\n";

            //Checks if this file was modified
            if (oldDateModified.count(filePath)) {
                //If the file is already there, compare its old and new timestamps.
                if (currentModifiedTime > oldDateModified[filePath]) {
                    newlyModifiedFiles.push_back(filePath);
                }
            } else {
                //If the file is new, add it to the modified list
                newlyModifiedFiles.push_back(filePath);
            }
        }
    }

    newModifiedFile.close();

    //If any files were modified, create a report file
    if (!newlyModifiedFiles.empty()) {
        cout << newlyModifiedFiles.size() << " file(s) have been modified or added." << endl;
        cout << "Creating report file: " << newlyDateModifiedTxt << endl;

        ofstream reportDateModified(newlyDateModifiedTxt, ios::trunc);

        if (!reportDateModified.is_open()) {
            cerr << "Error: Could not open report file - " << newlyDateModifiedTxt << endl;
        } else {
            for (size_t i = 0; i < newlyModifiedFiles.size(); ++i) {
                reportDateModified << newlyModifiedFiles[i] << "\n";
            }
            reportDateModified.close();
        }
    } else {
        cout << "No files have been modified since the last check." << endl;
    }

    cout << "Check complete! Date modified times are now up to date." << endl;
}

//A helper function to parse a given log file for a list of keywords.
void parse_log_file(const string& logFilePath, const vector<string>& keywords, const string& reportFile) {
    ifstream logFile(logFilePath);

    //Check if the log file can be opened. Permissions might be an issue.
    if (!logFile.is_open()) {
        cerr << "Error: Could not open log file - " << logFilePath 
             << ". Try running with sudo permissions." << endl;
        return;
    }

    ofstream report(reportFile);
    if (!report.is_open()) {
        cerr << "Error: Could not create report file for writing - " << reportFile << endl;
        logFile.close();
        return;
    }

    cout << "Parsing " << logFilePath << " for keywords..." << endl;
    long matchCount = 0;
    string line;

    //Read the log file line by line
    while (getline(logFile, line)) {
        //Check if any of the keywords are present in the current line
        for (const string& keyword : keywords) {
            if (line.find(keyword) != string::npos) {
                report << line << "\n";
                matchCount++;
                break; // Keyword found, no need to check for others on this line
            }
        }
    }

    logFile.close();
    report.close();

    cout << "Log parsing complete. Found " << matchCount << " matching lines." << endl;
    cout << "Report saved to " << reportFile << endl;
}

//Parses the main system log (/var/log/syslog) for keywords.
void parse_system_logs(const vector<string>& keywords, const string& reportFile) {
    parse_log_file("/var/log/syslog", keywords, reportFile);
}

//Parses the kernel log (/var/log/kern.log) for keywords.
void parse_kernel_logs(const vector<string>& keywords, const string& reportFile) {
    parse_log_file("/var/log/kern.log", keywords, reportFile);
}
