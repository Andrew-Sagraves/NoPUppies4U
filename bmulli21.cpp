//The include statements will be cleaned up later
#include "bmulli21.h"
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <ctime>
#include <vector>
#include <dirent.h>
#include <filesystem>

using namespace std;

//Functions have been made void to avoid conflicts with other files

//Checks if any files in a directory have been modified since the last check
//bool check_date_modified(string filename){
void check_date_modified(string filename){

    struct stat fileStat;
    
    //This is a test at the moment, will be changed later
    if(stat(filename.c_str(),&fileStat) < 0){ //Check if file exists
        cout << "Error getting file information" << endl;
        return;
    }else{ //File exists, print last modified date
        cout << "File: " << filename << endl;
        cout << "Last modified: " << ctime(&fileStat.st_mtime) << endl;
    }

    return;
}

//Creates a file with all the files that have been modified since the last check
void create_file(vector<string> files){
    //Takes in a vector of files and creates a file with the names of the files that have been modified since the last check and the date they were last modified
    

    return;
}

//Creates a vector of all files in a directory
//vector<string> create_vector(string directory){
void create_vector(string directory){
    vector<string> files;
    //Takes in a directory and creates a vector of all files in that directory
    //This will be used to check if any files in the directory have been modified since the last check
    filesystem::path path(directory);
    if(!filesystem::exists(path) || !filesystem::is_directory(path)){
        cout << "Directory does not exist" << endl;
        return;
    }


    //return files;
    return;
}

//Checks if any files in a directory have been modified since the last check
void check_directory(vector<string> files){



    return;
}

//Recursively checks a directory and subdirectories for modified files
void check_all_directories(string directory){

    //This function will check all directories in the current directory for modified files
    //It will create a vector of all files in each directory and pass it to the check_directory function

    return;
}