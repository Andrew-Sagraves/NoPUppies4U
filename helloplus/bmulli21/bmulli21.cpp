#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

using namespace std;

// Function to list all files in a given directory and return their names in a vector
vector<string> ListFiles(const string& directoryPath) {
    vector<string> fileNames;
    for (const auto& entry : filesystem::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            fileNames.push_back(entry.path().filename().string());
        }
    }
    return fileNames;
}

int main(int argc, char const *argv[])
{
  if (argc < 2) {
    cerr << "Please provide a directory path." << endl;
    return 1;
  }

  string directoryPath = argv[1];
  vector<string> files = ListFiles(directoryPath); 
  for (const string& fileName : files) {
      cout << fileName << endl;
  }

  return 0;
}
