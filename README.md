# NoPUppies4U
## Group Members
- Kaden Bissonnette: Kaden-Bissonnette
- Andrew Sagraves:   Andrew-Sagraves
- Jason Dong:        wujason1030
- Bryan Mullins:     bmulli17
- Harrison Clark:    hclark37

## Logo
<img width="512" height="512" alt="image" src="https://github.com/user-attachments/assets/64071203-8238-4436-bfb6-3cba23ba1c3a" />


## Product Description
NoPUppies4U is a light-weight red/blue team tool that will scan your filesystem for common misconfigurations and vulnerabilities. This project looks at cronjobs and the permission for the files that all the cronjobs look at, looking at the currently issued ssh-keys, and much more (see `nopuppies4u -h` for full list of functionality). This project will produce output to .txt files or directly to the terminal dependent on the function called. It can also be installed/uninstalled to match normal unix utility as any other unix function (like `ls`).

## Instructions
1. Pull the project from GitHub into your Ubuntu OS:
   ```
   git clone https://github.com/Andrew-Sagraves/NoPUppies4U.git
   ```
3. Installing the project:
   ```
   chmod u+x ./install.sh
   ./install.sh
   ```
4. Compile the project using the Makefile:
   ```make```
5. Running the project(Require use of sudo/root for certain commands):
   ```
   sudo nopuppies4u
   # Use nopuppies4u -h for help
   ```
6. Look at the contents of PATH.txt for program output (cat PATH.txt)
7. Logs are at "/var/log/NoPUppies4U/".

## [License](https://github.com/Andrew-Sagraves/NoPUppies4U/blob/main/LICENSE)
