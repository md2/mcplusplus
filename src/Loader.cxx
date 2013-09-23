#include <string.h>
#include <fnmatch.h>
#include <fstream>
#include "common.h"

#define __use_fopen__

using namespace std;

int LoadDigestsFromFile(const char *filename, const char *filter) {
	size_t len = 0;
	bool fnm = false;
	if (filter) {
		len = strlen(filter);
		if (strpbrk(filter, "*?[") != NULL)
			fnm = true;
	}
#ifdef __use_fopen__
	FILE *fi = fopen(filename, "rt");
#else
	ifstream fi(filename);
#endif
	if (!fi) return -1;
	char line[256];
	char *p;
	char *login, *hex;
#ifdef __use_fopen__
	while (fgets(line, sizeof(line) - 1, fi)) {
#else
	while (fi.getline(line, sizeof(line) - 1) && fi.gcount() > 1) {
#endif
		p = line;
		for (login = p++; *p && *p != ':'; p++);
		*p++ = '\0';
		for (hex = p; *p && isxdigit(*p); p++);
		*p++ = '\0';
		if (filter && (fnm ? fnmatch(filter, hex, FNM_NOESCAPE) : strncmp(hex, filter, len)))
			continue;
		hf->addEntry(hex, login);
	}
#ifdef __use_fopen__
	fclose(fi);
#endif
	return 0;
}
int LoadRulesFromFile(const char *filename) {
#ifdef __use_fopen__
	FILE *fi = fopen(filename, "rt");
#else
	ifstream fi(filename);
#endif
	if (!fi) return -1;
	char line[256];
#ifdef __use_fopen__
	while (fgets(line, sizeof(line) - 1, fi)) {
#else
	while (fi.getline(line, sizeof(line) - 1)) {
#endif
		if (*line == '#' || *line == ';')
			continue;
		chomp(line);
		if (*line == '_' && !strcmp(line, "__END__"))
			break;
		if (*line != '\0') {
			rules.push_back(line);
		}
	}
#ifdef __use_fopen__
	fclose(fi);
#endif
	return 0;
}
