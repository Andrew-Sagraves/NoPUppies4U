# Sprint 2
- Name:        Bryan Mullins
- Github ID:   bmulli17
- Group Name:  NoPUppies4U
## What you planned to do
- Parsing through system logs
- Parsing kernel logs
- Update date modified files logs
## What you did not do
- Nothing, I was able to accomplish my goals
- Did not get ignoring hidden folders working
## What problems you encountered
- Permissions kept making testing the funcitons a bit annoying
- Parsing the correct information in the system and kernel logs and filtering unwanted logs
- Dealing with sybolic links causing infinite loop issues with the check directories function
## Issues you worked on
- [#17](https://github.com/Andrew-Sagraves/NoPUppies4U/issues/17)
## Files you worked on
- bmulli21.cpp
- bmulli21.h
## Use of AI and/or 3rd party software
I used Github Copilot to help provide small code snippets and suggestions. I also used Google Gemini 2.5 to help with some logic errors and bug fixing.
## What you accomplished
I was able to create two parsing functions that check system and kernel logs for keywords. I also updated my check_directory_for_changes() to allow for a flag system to overwrite some parts of the function such as checking the entire system from the root folder, overwriting the date_modified.txt, and ingnoring hidden folders (Did not get implemented). I wish I was able to fully implement the ignoring hidden folders this sprint as testing my functions sometimes took a while due to the amount of hidden folders on the system. I want to create more flags to allow the checking function to reduce overhead and run faster by ignoring certain files or directories when specified.