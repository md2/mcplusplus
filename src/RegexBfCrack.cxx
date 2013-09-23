#include <sys/time.h>
#include <string.h>
#include <fstream>
#include "common.h"

using namespace std;

void RegexBfCrack() {
	long long passwordsFound = 0,
	          passwordsChecked = 0;
	struct timeval t0, t1;
	double lapse;
	const char *rule;
	long long ticks = 0;
	gettimeofday(&t0, NULL);
	SignalHandler *ha = new SignalHandler;
	RulePreprocessor * rpp = new RulePreprocessor;
	char buffer[256];
	size_t len;
	while ((rule = rpp->NextRule()) != NULL) {
		if (ticks != ha->Ticks()) {
			if (ha->Interrupted())
				goto _g0;
			if (tty.ReadKey() != -1) {
				while (tty.ReadKey() != -1) ;
				gettimeofday(&t1, NULL);
				lapse = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1000000.0;
				printf(_("found: %-16d - trying: %-16s - speed: %-9.3f p/s\n"),
				       (int)passwordsFound, rule ? rule : "", (float)passwordsChecked / lapse);
			}
			ticks = ha->Ticks();
		}
		int found = hf->computeAndFind(rule, rpp->getLength());
		passwordsChecked++;
		if (found) {
			string data = hf->getDataAndEraseFound();
			printf(_("%-16s (%s)\n"), rule, data.c_str());
			fstream pot(output, ios::out | ios::app);
			pot << hf->toPr() << ':' << rule << endl;
			pot.close();
			passwordsFound++;
			if (hf->empty())
				goto _g0;
		}
	}
_g0:
	delete ha;
	delete rpp;
	gettimeofday(&t1, NULL);
	lapse = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1000000.0;
	printf("\n");
	printf(_("              ===================== Summary =====================\n"));
	printf(_("                PASSWORDS FOUND:               %d/%d\n"), passwordsFound, hf->size() + passwordsFound);
	printf(_("              PASSWORDS CHECKED:               %lld\n"), passwordsChecked);
	printf(_("                          SPEED:               %.3f p/s\n"), (float)passwordsChecked / lapse);
	printf(_("                          LAPSE:               %.3f s\n"), (float)lapse);
}

