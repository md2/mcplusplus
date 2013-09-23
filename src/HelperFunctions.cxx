#include "HelperFunctions.h"
#include <string.h>
#include <string>

char *chomp(char *str) {
	char *p;
	if ((p = strrchr(str, '\n')) != NULL)
		*p = '\0';
	if ((p = strrchr(str, '\r')) != NULL)
		*p = '\0';
	return str;
}

int hex(char x) {
	if (x >= '0' && x <= '9')
		return x - '0';
	else if (x >= 'A' && x <= 'F')
		return x - 'A' + 10;
	else if (x >= 'a' && x <= 'f')
		return x - 'a' + 10;
	return -1;
}

