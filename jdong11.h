#ifndef JDONG11_H
#define JDONG11_H

using namespace std;

// check if /etc/apt/sources.list URLs is valid
void check_sources_list();
// check all users in sudo group and print if out
void check_sudo();
// check if system is up-to-date
void check_sys_updated();
// checking firewall of system
void check_ufw();
// setting up Network Intrusion Detection system
//void check_snort();
// checking for ncat backdoor
// void ncat_backdoor();
#endif