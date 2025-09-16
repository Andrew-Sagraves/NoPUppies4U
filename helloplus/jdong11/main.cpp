#include "main.h"
#include <unistd.h>
#include <cstdio>

Test::Test() {}

void Test::permTest() {
    if (geteuid() != 0) {
		fprintf(stderr, "You must be root to run this program.\n");
	}
}

void Test::openFile() {
    FILE *sourceFile = fopen("/etc/apt/sources.list", "r");
	if (sourceFile == NULL) {
		perror("/etc/apt/sources.list");
	}
}