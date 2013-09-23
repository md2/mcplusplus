#include <sys/time.h>
#include <string.h>
#include <fstream>
#include "common.h"

#define __strategy__ 2

using namespace std;

void RandomCrack(const char *charset, size_t minlen, size_t maxlen, const char *rule) {
	long long passwordsFound = 0,
	          passwordsChecked = 0;
	struct timeval t0, t1;
	double lapse;
	long long ticks = 0;
	char str[maxlen];
	const char *res;
	int i;
	char *pos;
	int found;
	int cslen = strlen(charset);
	printf(_("charset: %s\n"), charset);
	RandomNumberGenerator rng;
	gettimeofday(&t0, NULL);
	RuleApplicator *ra = new RuleApplicator;
	SignalHandler *ha = new SignalHandler;
	for (int l = minlen; l <= maxlen; l++) {
		str[l] = '\0';
		unsigned long long max = 1;
		for (i = 0; i < l; i++)
			max *= cslen;
#if __strategy__ == 2
		for (i = 0; i < l; i++)
			str[i] = charset[rng.getRandom(cslen)];
#endif
		while (max--) {
#if __strategy__ == 1
			for (i = 0, pos = str; i < l; i++, pos++)
				*pos = charset[rng.getRandom(cslen)];
#elif __strategy__ == 2
			str[rng.getRandom(l)] = charset[rng.getRandom(cslen)];
#endif
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
	delete ra;
	delete ha;
	gettimeofday(&t1, NULL);
	lapse = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1000000.0;
	printf("\n");
	printf(_("              ===================== Summary =====================\n"));
	printf(_("                PASSWORDS FOUND:               %d/%d\n"), passwordsFound, hf->size() + passwordsFound);
	printf(_("              PASSWORDS CHECKED:               %lld\n"), passwordsChecked);
	printf(_("                          SPEED:               %.3f p/s\n"), (float)passwordsChecked / lapse);
	printf(_("                          LAPSE:               %.3f s\n"), (float)lapse);
}

