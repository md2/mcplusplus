#include <sys/time.h>
#include <string.h>
#include <fstream>
#include "common.h"

#define __use_fopen__

using namespace std;

stringlist dicts;

void DictionaryCrack(const char *dbfile) {
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
	Db *db = dbfile ? new Db(dbfile) : NULL;
#ifdef __use_fopen__
	FILE *fi;
#else
	ifstream fi;
#endif
	char buffer[256];
	size_t len;
	stringlistiterator const end = dicts.end();
	while ((rule = rpp->NextRule()) != NULL) {
		for (stringlistiterator i = dicts.begin(); i != end; i++) {
#ifdef __use_fopen__
			fi = fopen(i->c_str(), "rt");
			if (!fi) continue;
			while (fgets(buffer, sizeof(buffer) - 1, fi)) {
#else
			fi.open(i->c_str());
			if (!fi) continue;
			while (fi.getline(buffer, sizeof(buffer) - 1)) {
#endif
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
				chomp(buffer);
				len = strlen(buffer);
				res = ra->Apply(rule, buffer, len);
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
						if (db) db->updateStatisticsForRule(rule);
						if (hf->empty())
							goto _g0;
					}
				}
			}
#ifdef __use_fopen__
			fclose(fi);
			fi = NULL;
#else
			fi.close();
#endif
		}
	}
	goto _g1;
_g0:
#ifdef __use_fopen__
	if (fi) fclose(fi);
#else
	fi.close();
#endif
_g1:
	delete ha;
	delete ra;
	delete rpp;
	if (db) delete db;
	gettimeofday(&t1, NULL);
	lapse = (t1.tv_sec - t0.tv_sec) + (t1.tv_usec - t0.tv_usec) / 1000000.0;
	printf("\n");
	printf(_("              ===================== Summary =====================\n"));
	printf(_("                PASSWORDS FOUND:               %d/%d\n"), passwordsFound, hf->size() + passwordsFound);
	printf(_("              PASSWORDS CHECKED:               %lld\n"), passwordsChecked);
	printf(_("                          SPEED:               %.3f p/s\n"), (float)passwordsChecked / lapse);
	printf(_("                          LAPSE:               %.3f s\n"), (float)lapse);
}

