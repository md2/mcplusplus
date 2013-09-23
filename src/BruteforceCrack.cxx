#include <sys/time.h>
#include <string.h>
#include <fstream>
#include "common.h"

using namespace std;

void BruteforceCrack(const char *charset, size_t minlen, size_t maxlen, const char *startfrom, const char *rule) {
	long long passwordsFound = 0,
	          passwordsChecked = 0;
	struct timeval t0, t1;
	double lapse;
	long long ticks = 0;
	size_t cs_len = strlen(charset);
	int pos[maxlen];
	char str[maxlen];
	const char *res;
	int i;
	printf(_("charset: %s\n"), charset);
	gettimeofday(&t0, NULL);
	RuleApplicator *ra = new RuleApplicator;
	SignalHandler *ha = new SignalHandler;
	bool initialized = false;
	if (startfrom != NULL && strlen(startfrom) == minlen) {
		int k;
		for (k = 0; k < minlen; k++) {
			const char *p;
			if ((p = strchr(charset, startfrom[k])) == NULL)
				break;
			pos[k] = p - charset;
		}
		if (k == minlen) {
			strcpy(str, startfrom);
			initialized = true;
		}
	}
	for (int l = minlen; l <= maxlen; l++) {
		int found;
		if (initialized)
			initialized = false;
		else {
			for (i = 0; i < l; i++) {
				pos[i] = 0;
				str[i] = charset[0];
			}
			str[l] = '\0';
		}
		bool firsttime = true;
		while (true) {
			if (firsttime)
				firsttime = false;
			else {
				for (i = l - 1; i >= 0; i--) {
					pos[i]++;
					if (pos[i] <= cs_len - 1) {
						str[i] = charset[pos[i]];
						break;
					}
					pos[i] = 0;
					str[i] = charset[0];
				}
				if (i < 0)
					break;
			}
			if (ticks != ha->Ticks()) {
				if (ha->Interrupted())
					goto _g0;
				if (tty.ReadKey() != -1) {
					while (tty.ReadKey() != -1) ;
					gettimeofday(&t1, NULL);
					lapse = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1000000.0;
					printf(_("found: %-16d - trying: %-16s - speed: %-9.3f p/s\n"),
					       (int)passwordsFound, res ? res : "", (float)passwordsChecked / lapse);
				}
				ticks = ha->Ticks();
			}
			size_t l_ = l;
			res = rule ? ra->Apply(rule, str, l_) : str;
			if (res) {
				found = hf->computeAndFind(res, l_);
				passwordsChecked++;
				if (found) {
					string data = hf->getDataAndEraseFound();
					printf(_("%-16s (%s)\n"), res, data.c_str());
					fstream pot(output, ios::out | ios::app);
					pot << hf->toPr() << ':' << res << endl;
					pot.close();
					passwordsFound++;
					if (hf->empty())
						goto _g0;
				}
			}
		}
	}
_g0:
	delete ha;
	delete ra;
	gettimeofday(&t1, NULL);
	lapse = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1000000.0;
	printf("\n");
	printf(_("              ===================== Summary =====================\n"));
	printf(_("                PASSWORDS FOUND:               %d/%d\n"), passwordsFound, hf->size() + passwordsFound);
	printf(_("              PASSWORDS CHECKED:               %lld\n"), passwordsChecked);
	printf(_("                          SPEED:               %.3f p/s\n"), (float)passwordsChecked / lapse);
	printf(_("                          LAPSE:               %.3f s\n"), (float)lapse);
}

