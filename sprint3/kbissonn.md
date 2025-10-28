# Sprint 2
- Name:        Kaden Bissonnette
- Github ID:   Kaden-Bissonnette
- Group Name:  NoPUppies4U
## What you planned to do
- Work on verbose functionality for checking cron jobs
- Provide the user with the file permissions of all the sudoer files 
- Flag that sudoer files that are non-traditional (440)
## What you did not do
- I was able to complete everything that I set out to do this sprint. I would have liked to find something else to add to the project, but was unable to find anything that proved useful.
## What problems you encountered
- Some of the pushed code was not 100% working, leading to it being harder to compile and test my code without running my code without the shared main file
## Issues you worked on
- [#23](https://github.com/Andrew-Sagraves/NoPUppies4U/issues/23)
- [#29](https://github.com/Andrew-Sagraves/NoPUppies4U/issues/29)
## Files you worked on
- kbissonn.cpp
- kbissonn.h
## Use of AI and/or 3rd party software
Most of what I wrote for this sprint was just reinterprited versions of code that I already wrote for previous sprints to work towards a more specific goal, but I did use AI to help write some of the new functionality as I was accepting the suggested lines while I was codeing beside it just to speed up the process a litle.
## What you accomplished
I added a verbose function to checking the cron jobs that allows the user of the program to see every cron job that they have on their system, while still highlighting which ones are world writeable and should be changed in order to prevent a security breach. I also created a function that looks at the permissions of the sudoer files to make sure that they are properly set and cannot be altered by unwanted parties.