sprint 2
- Name:        Andrew Sagraves
- Github ID:   Andrew-Sagraves 
- Group Name:  NoPUppies4U
## What you planned to do
- SUID binary auditing
- Check ssh keys for worls-writable permissions
- check for users with no passwrds
## What you did not do
- Nothing, I was able to accomplish my goals
## What problems you encountered
- Permissions kept making testing the funcitons a bit annoying
- Parsing the correct information in the system and kernel logs and filtering unwanted logs
- Dealing with sybolic links causing infinite loop issues with the check directories function
## Issues you worked on
Check for passwordless SUDO access #22
Create a program that checks ssh keys for world-writiable permissions #10
SUID Binary auditing #9
## Files you worked on
- asagrave.cpp
- asagrave.h
## Use of AI and/or 3rd party software
I used chatgpt to help guide me through code
## What you accomplished
I created a set of system auditing functions to check for common security risks. The code includes utilities for directory creation, logging, and recursive file searches. Three main audits were implemented: one checks for passwordless sudo access, another scans for world-writable SSH keys, and the third searches for SUID binaries in key system directories. These features work together to identify potential privilege or configuration issues. In future updates, I plan to add more checks and options to control which scans run for better efficiency.
