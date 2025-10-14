# Sprint 2
- Name:        Kaden Bissonnette
- Github ID:   Kaden-Bissonnette
- Group Name:  NoPUppies4U
## What you planned to do
- Implement opening and looking at the Sudoers file to get every user that can use sudo
- Fix a logic error in the check_cron_jobs() function where it would print every called file rather than just those that are world writable.
- Implement looking at the check_cron_jobs() for world writeable directories as well
## What you did not do
- I was able to complete everything that I set out to do this sprint
## What problems you encountered
- I had to look into some functions that I was uncomfortable with, such as those used for checking file permissions
- Some of the pushed code was not 100% working, leading to it being harder to compile and test my code without running my code without the shared main file
## Issues you worked on
- [#4](https://github.com/Andrew-Sagraves/NoPUppies4U/issues/4)
- [#11](https://github.com/Andrew-Sagraves/NoPUppies4U/issues/11)
## Files you worked on
- kbissonn.cpp
- kbissonn.h
## Use of AI and/or 3rd party software
I used copilot to help provide and explain some c++ functions that could do what I wanted to do in the code. For example, I asked it what functions could be used to look at a file's permissions. I also used it to get some background on what sudoer files look like in order to figure out how I could pull the users from within the file.
## What you accomplished
I fixed the check_cron_jobs() function to check the other permissions for files rather than the current user's permission. Fully implemented the check_sudoers() function that looks at the sudoers file and provides all of the users that have any sudo access within the filesystem.
