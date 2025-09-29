# Sprint n (1, 2, 3, or 4)
- Harrison Clark
- hclark37
- NoPuppies4U

### What you planned to do
Issue # 6 (https://github.com/Andrew-Sagraves/NoPUppies4U/issues/6)
- Create a main.cpp that can accept flags
- Make a make file and install and uninstall program for the program
- Make a help/man file and a way to use it

### What you did not do
- Did not implement specifically every goal flag that we wanted, because they weren't finished by other people

### What problems you encountered
- Learning new formats, like the language/syntax for man page files, and some basic bash scripting, was difficult but rewarding.
- Learning new C++ libraries and how to use them like getopt.h.
- Managing other people, because I was working with main directly and its features, which relied on other people's code.

### Issues you worked on
Issue # 6 (https://github.com/Andrew-Sagraves/NoPUppies4U/issues/6)

### Files you worked on
- main.cpp
- install.sh
- uninstall.sh
- man/nopuppies4u.1
- Makefile

### Use of AI and/or 3rd party software
- Used it to help teach myself things, like, "Explain how this tool works" or "what format does this kind of file go in?" or "what is its syntax?" using ChatGPT specifically.

### What you accomplished
I was able to create a main.cpp that could accept flags into it, and was deterministic- like, you can't pass a flag that doesn't either cause it to have a controlled close, or succeed. That was one of my biggest goals.
I created a Makefile that you could use to compile everyone's work. I made a manual page that could be ran using the man command, which detailed the program's purposes. 
I made an install script that would build the program, install the program into your path, and then install a man page so it can be ran without pointing to the original build directory's version of the file.
I also made an uninstall script, that would remove these changes. Overall, I think I succeeded in many of my goals, and the program ended up effective, in a big part because of my work. 
My biggest accomplishment was successfully combining everyone's parts to run under my flag system, so that we had a final working program.
