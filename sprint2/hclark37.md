# Sprint 2
- Harrison Clark
- hclark37
- NoPuppies4U

### What you planned to do
Issue # 20 (https://github.com/Andrew-Sagraves/NoPUppies4U/issues/20)
- Implement functionality from other people's programs
- Add the new functions that people have added for sprint 2
- Implement providable arguments for those programs, if possible for the function  

Issue # 19 (https://github.com/Andrew-Sagraves/NoPUppies4U/issues/19)
- Test to see if there are users without passwords
- Also checks for locked users and empty password users 

Issue # 18 (https://github.com/Andrew-Sagraves/NoPUppies4U/issues/18)
- Create a script that creates an intentionally vulnerable environment
- Uses info from all the other people's code to make vulnerabilities that will be flagged by them

### What you did not do
- I did everything that I intended to this sprint. I kind of had an intention of creating a weak password tester- one that would check to see whether users have insecure passwords or not via a basic amount of bruteforcing (maybe?)- but I decided to not do that for this sprint and put it off for another. There were some things I couldn't fully integrate because they simply weren't done by other people by close enough to the deadline, like fully featured flags.

### What problems you encountered
- Learning how to write BASH scripts is a little difficult, and I struggled with it a bit.
- Learning how everybody elses' programs worked to create a test environment was difficult for me too. It required a lot of reading code and googling different things.
- Creating a virtual machine wasn't the easiest thing, setting up a test environment that worked. But I did do it, and I'm thinking from there that I might create a new program next sprint that focuses on creating a virtual machine for users.
- Being able to only do things when other people have them done. There were some things I couldn't fully integrate because they simply weren't done by other people by close enough to the deadline.
  
### Issues you worked on
Issue # 18 (https://github.com/Andrew-Sagraves/NoPUppies4U/issues/18)
Issue # 19 (https://github.com/Andrew-Sagraves/NoPUppies4U/issues/19)
Issue # 20 (https://github.com/Andrew-Sagraves/NoPUppies4U/issues/20)
All part of this milestone: https://github.com/Andrew-Sagraves/NoPUppies4U/milestone/1 (sprint 2)

### Files you worked on
- main.cpp
- vulnerabilty_maker.sh
- hclark37.h
- hclark37.cpp
- man/nopuppies4u.1

### Use of AI and/or 3rd party software
- Used it to help teach myself things, like, "Explain how cron jobs work" or "How does the /etc/shadow store no/locked/etc passwords?" using Deepseek specifically.

### What you accomplished
My biggest goal this sprint was to create a script that was able to create a testing environment. Part of this, of course, is the part that I can't really show in commits, is the work of setting up the virtual machine and all of that and then testing the script on it. Ultimately, however, I was able to complete this and test that my BASH script functions correctly. 
Another goal of my part was to create a script that is able to test whether there exists users on the machine that don't have passwords or that are locked or have empty passwords. This took some research about how the way that /etc/shadow works, but I was able to figure it out. 
I also worked to implement other people's work into the program overall. This is a continual process for every sprint, as more people, including myself, add features to the program. 
