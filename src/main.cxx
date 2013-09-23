#include <stdlib.h>
#include <string>
#include <string.h>
#include <popt.h>
#include "common.h"

char *output = NULL;

using namespace std;

void usage(poptContext ctx, int exitcode) {
	poptPrintUsage(ctx, stderr, 0);
	exit(exitcode);
}

int main(int argc, const char *argv[]) {
	a_md5 = new MD5;
	char *algo = strdup("md5"), *mode = NULL,
	      *charset = strdup("0-9a-zA-Z"),
	       *startfrom = NULL, *length = strdup("1-32"),
	        *db = strdup("mc++.db"),
	         *word_filter = NULL, *hash_filter = NULL,
	          *in_between = NULL;
	char *algolist = strdup(algos.listOf().c_str());
	int collect_statistics = 0;
	stringlist l1;
	size_t minlen, maxlen;
	poptContext ctx;
	struct poptOption optionsTable[] = {
		{
			"algo", 'a', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &algo, 0,
			_("Message digest algorithm to use"), algolist
		},
		{
			NULL, 'm', POPT_ARG_STRING, &mode, 0,
			_("Crack mode (d=dictionary, b=bruteforce, B=extended bruteforce, r=random)"), "d|b|B|r"
		},
		{
			"output", 'o', POPT_ARG_STRING, &output, 0,
			_("Output found passwords to <file> (default: \"mc++-<ALGO>.pot\")"), "<file>"
		},
		{
			"dictionary", 'd', POPT_ARG_STRING, NULL, 'd',
			_("Use <file> as a dictionary (dictionary mode)"), "<file>"
		},
		{
			"rules", 'r', POPT_ARG_STRING, NULL, 'r',
			_("Use <file> as a rulefile (dictionary and extended bruteforce mode)"), "<file>"
		},
		{
			"charset", 'c', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &charset, 0,
			_("Charset to use (bruteforce and random mode)"), "CHARSET"
		},
		{
			"length", 'l', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &length, 0,
			_("Limit password length (bruteforce and random mode)"), "MINLEN[-MAXLEN]"
		},
		{
			"start-from", 's', POPT_ARG_STRING, &startfrom, 0,
			_("Password to start bruteforcing with (bruteforce mode)"), "PASSWORD"
		},
		{
			"collect-statistics", '\0', POPT_ARG_VAL, &collect_statistics, 1,
			_("Collect statistics (dictionary mode)"), NULL
		},
		{
			"db", '\0', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &db, 0,
			_("Use database in <file>"), "<file>"
		},
		{
			"start", '\0', POPT_ARG_STRING, NULL, 1,
			_(""), NULL
		},
		{
			"end", '\0', POPT_ARG_STRING, NULL, 2,
			_(""), NULL
		},
		{
			"word-filter", '\0', POPT_ARG_STRING, &word_filter, 0,
			_("Apply <rule> before checking (bruteforce and random bruteforce mode)"), "<rule>"
		},
		{
			"hash-filter", '\0', POPT_ARG_STRING, &hash_filter, 0,
			_("Load only hashes matching <filter>"), "<filter>"
		},
		{
			"in-between", 'i', POPT_ARG_STRING, &in_between, 0,
			_("String to insert in between words (two-dictionary mode)"), "STRING"
		},
		POPT_AUTOHELP
		{ NULL, '\0', 0, NULL, 0, NULL, NULL}
	};

	ctx = poptGetContext(NULL, argc, argv, optionsTable, 0);
	poptSetOtherOptionHelp(ctx, _("[OPTIONS]* <password-files>"));
	if (argc < 2) {
		poptPrintUsage(ctx, stderr, 0);
		exit(-1);
	}
	int opt;
	const char *optarg;
	while ((opt = poptGetNextOpt(ctx)) >= 0) {
		switch (opt) {
			case 'd':
				optarg = poptGetOptArg(ctx);
				dicts.push_back(optarg);
				delete optarg;
				break;
			case 'r':
				optarg = poptGetOptArg(ctx);
				l1.push_back(optarg);
				delete optarg;
				break;
			case 1:
				optarg = poptGetOptArg(ctx);
				ds.push_back(optarg);
				delete optarg;
				break;
			case 2:
				optarg = poptGetOptArg(ctx);
				de.push_back(optarg);
				delete optarg;
				break;
		}
	}
	if (opt < -1) {
		fprintf(stderr, _("Error: %s: %s\n"), poptBadOption(ctx, POPT_BADOPTION_NOALIAS), poptStrerror(opt));
		exit(-1);
	}
	char *p;
	if ((p = strchr(length, '-')) == NULL)
		maxlen = minlen = atoi(length);
	else {
		*p++ = '\0';
		minlen = atoi(length);
		maxlen = atoi(p);
	}
	if (startfrom)
		minlen = strlen(startfrom);
	if (!mode || strcmp(mode, "d") != 0 && strcmp(mode, "b") != 0 &&
	        strcmp(mode, "B") != 0 && strcmp(mode, "r") != 0 &&
	        strcmp(mode, "2") != 0)
		usage(ctx, -1);
	if (!strcmp(mode, "d") && (dicts.empty() || l1.empty()))
		usage(ctx, -1);
	hf = algos.Find(algo);
	if (!hf)
		usage(ctx, -1);
	if (!output) {
		char buffer[256];
		strcpy(buffer, "mc++-");
		strcat(buffer, hf->getName());
		strcat(buffer, ".pot");
		output = strdup(buffer);
	}
	printf(_("algo: %s\n"), hf->getName());
	while ((optarg = poptGetArg(ctx)) != NULL)
		LoadDigestsFromFile(optarg, hash_filter);
	if (hf->empty()) {
		fprintf(stderr, _("Error: No digests!\n"));
		exit(-1);
	}
	printf(_("%d different digests loaded.\n"), hf->size());
	poptFreeContext(ctx);
	if (!strcmp(mode, "b"))
		BruteforceCrack(Charset(charset).getSequence().c_str(), minlen, maxlen, startfrom, word_filter);
	else if (!strcmp(mode, "r"))
		RandomCrack(Charset(charset).getSequence().c_str(), minlen, maxlen, word_filter);
	else if (!strcmp(mode, "B")) {
		for (stringlistiterator i = l1.begin(); i != l1.end(); i++)
			LoadRulesFromFile(i->c_str());
		if (rules.empty()) {
			fprintf(stderr, _("Error: No rules!\n"));
			exit(-1);
		}
		printf(_("%d rules loaded.\n"), rules.size());
		RegexBfCrack();
	} else if (!strcmp(mode, "d")) {
		for (stringlistiterator i = l1.begin(); i != l1.end(); i++)
			LoadRulesFromFile(i->c_str());
		if (rules.empty()) {
			fprintf(stderr, _("Error: No rules!\n"));
			exit(-1);
		}
		printf(_("%d rules loaded.\n"), rules.size());
		DictionaryCrack(collect_statistics ? db : NULL);
	} else if (!strcmp(mode, "2")) {
		for (stringlistiterator i = l1.begin(); i != l1.end(); i++)
			LoadRulesFromFile(i->c_str());
		if (rules.empty()) {
			fprintf(stderr, _("Error: No rules!\n"));
			exit(-1);
		}
		printf(_("%d rules loaded.\n"), rules.size());
		TwoDictionaryCrack(word_filter, in_between);
	}
	return 0;
}

