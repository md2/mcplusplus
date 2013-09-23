#include <sys/time.h>
#include <string.h>
#include <fstream>
#include "common.h"

using namespace std;

stringlist ds, de;

void TwoDictionaryCrack(const char *rule2, const char *in_between) {
	long long passwordsFound = 0,
	          passwordsChecked = 0;
	struct timeval t0, t1;
	double lapse;
	const char *rule;
	const char *res;
	long long ticks = 0;
	gettimeofday(&t0, NULL);
	SignalHandler *ha = new SignalHandler;
	RuleApplicator *ra = new RuleApplicator;
	RulePreprocessor * rpp = new RulePreprocessor;
	FILE *fi, *fj;
	char buffer[1024], buffer_s[256], buffer_e[256];
	size_t len;
	stringlistiterator const end = ds.end();
	stringlistiterator const end2 = de.end();
	while ((rule = rpp->NextRule()) != NULL) {
		for (stringlistiterator i = ds.begin(); i != end; i++) {
			fi = fopen(i->c_str(), "rt");
			if (!fi) continue;
			while (fgets(buffer_s, sizeof(buffer_s) - 1, fi)) {
				for (stringlistiterator j = de.begin(); j != end2; j++) {
					fj = fopen(j->c_str(), "rt");
					if (!fj) continue;
					while (fgets(buffer_e, sizeof(buffer_e) - 1, fj)) {
						if (ticks != ha->Ticks()) {
							if (ha->Interrupted())
								goto _g0;
							if (tty.ReadKey() != -1) {
								while (tty.ReadKey() != -1) ;
								gettimeofday(&t1, NULL);
								lapse = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1000000.0;
								printf(_("found: %-16d - trying: %-16s - rule: %-16s - speed: %-9.3f p/s\n"),
								       (int)passwordsFound, res ? res : "", rule ? rule : "", (float)passwordsChecked / lapse);
							}
							ticks = ha->Ticks();
						}
						chomp(buffer_s);
						chomp(buffer_e);
						strcpy(buffer, buffer_s);
						if (in_between) strcat(buffer, in_between);
						strcat(buffer, buffer_e);
						len = strlen(buffer);
						res = ra->Apply(rule, buffer, len);
						if (res && rule2) {
							strcpy(buffer, res);
							res = ra->Apply(rule2, buffer, len);
						}
						if (res) {
							int found = hf->computeAndFind(res, len);
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
					fclose(fj);
					fj = NULL;
				}
			}
			fclose(fi);
			fi = NULL;
		}
	}
	goto _g1;
_g0:
	if (fi) fclose(fi);
	if (fj) fclose(fj);
_g1:
	delete ha;
	delete ra;
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

