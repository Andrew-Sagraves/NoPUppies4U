# Sprint 1
- Name:        Kaden Bissonnette
- Github ID:   Kaden-Bissonnette
- Group Name:  NoPUppies4U
## What you planned to do
- Open and look at all cronjob files for called files
- Look at the called files and see their corrisponding permisions
- Look at sudoers files
## What you did not do
- Unable to complete looking at the sudoers files due to the cronjob files taking longer than expected
## What problems you encountered
- Making an incorrect estimate of the time to complete my assigned issues
- Only pushing when my code was fully functional made it harder tor Harrison to add to his code, but I did not was to push code that gave compile errors
- Running into a lot of comile errors due to using some functions that I am less comfortable with
## Issues you worked on
- [#2](https://github.com/Andrew-Sagraves/NoPUppies4U/issues/2)
- [#4](https://github.com/Andrew-Sagraves/NoPUppies4U/issues/4)
## Files you worked on
- kbissonn.cpp
- kbissonn.h
## Use of AI and/or 3rd party software
I had copilot on in the background and let it provide small code snippits, espessially for the recursion part to find the cron files as it was faster than me trying to stumble through figuring it out. I also used copilot for prototyping and providnig a little bit of guidence as to where to start. I also used ChatGPT for a lot for context into cronjobs and how they are layed out so I knew what specifically to look for in the files and the different types of cronjobs (daily, hourly, etc.).
## What you accomplished
I create all of the functionality for checking the cronjobs (files that run on a predetermined schedule) and looking at the files that they call to see if they are writable, and if they are then I print the file path to the console. I have also started on looking at the Sudoers file to see what can be ran as root with a similar technique as looking at the cronjobs, but it still has a lot of errors and does not compile (so I havent pushed it).