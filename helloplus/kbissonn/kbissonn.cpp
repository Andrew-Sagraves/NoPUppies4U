#include <iostream>
#include <string>
using namespace std;

void displayMenu() {
    cout << "Linux Auditor CLI Menu:" << endl;
    cout << "1. Set starting directory" << endl;
    cout << "2. Start audit" << endl;
    cout << "3. Exit" << endl;
}

int main() {
    string startPath;
    int choice;
    bool running = true;
    while (running) {
        displayMenu;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Clear newline from input buffer
        switch (choice) {
            case 1:
                cout << "Enter the starting directory path for the audit: ";
                getline(cin, startPath);
                cout << "Starting directory set to: " << startPath << endl;
                break;
            case 2:
                if (startPath.empty()) {
                    cout << "Please set the starting directory first." << endl;
                } else {
                    cout << "Audit will start from: " << startPath << endl;
                    // 
                    // Auditor logic can be added here
                    //
                }
                break;
            case 3:
                running = false;
                cout << "Exiting program." << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
        }
    }
    return 0;
}
