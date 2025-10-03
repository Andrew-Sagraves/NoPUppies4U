#ifndef JDONG11_H
#define JDONG11_H

using namespace std;

void check_sources_list();           // check if /etc/apt/sources.list URLs is valid
void check_sudo();                   // check all users in sudo group and print if out
void check_sys_updated();            // check if system is up-to-date

#endif