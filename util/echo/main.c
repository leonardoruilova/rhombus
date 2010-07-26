/* 
 * Copyright 2009, 2010 Nick Johnson 
 * ISC Licensed, see LICENSE for details
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	size_t n;

	for (n = 1; n < (size_t) argc; n++) {
		printf("%s ", argv[n]);
	}

	printf("\n");

	return 0;
}