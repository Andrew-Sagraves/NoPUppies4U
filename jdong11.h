#ifndef JDONG11_H
#define JDONG11_H

using namespace std;

int check_sources_list();           // check if /etc/apt/sources.list file is valid
int check_sudo();                   // check all users in sudo group
int check_sys_updated();            // check if system is up-to-date

#endif