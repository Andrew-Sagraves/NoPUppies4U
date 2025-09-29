# Sprint 1
- Name:        Bryan Mullins
- Github ID:   bmulli17
- Group Name:  NoPUppies4U
## What you planned to do
- Check date modified for files and given directory and output them to text file
- Compare dates from previous file to new file and output newly modified files
## What you did not do
- Nothing, I was able to accomplish my goals
## What problems you encountered
- Trying to create a recursive function to check all subdirectories caused some headaches
- Trying to pick the best data structure to make the program both efficient and simple took a while, but using a map to store the file path and the date modified time helped tremendously
- Having to completely start over due to my initial overcomplication of what was really needed
## Issues you worked on
- [#8](https://github.com/Andrew-Sagraves/NoPUppies4U/issues/8)
## Files you worked on
- bmulli21.cpp
- bmulli21.h
## Use of AI and/or 3rd party software
I used Github Copilot to help provide small code snippets and suggestions. I also used Google Gemini 2.5 to help with some logic errors and bug fixing.
## What you accomplished
I was able to create an all in one function to create a date modified list. This function also checked if there was already a date modified list and create a new file of all the newly changed files and ignoring the old ones.By using a map, I was able to store a key value pair of a file path and a date time much more efficiently than alternative methods.
I wish I could have added a flag system to the function to be able to overwrite the original date modified, ask the user for file names so they can customize it how they want, check only one level of the directory and not sublevels, and a few more functions. I am happy with what I was able to accomplish.