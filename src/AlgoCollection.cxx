#include <string.h>
#include "AlgoCollection.h"

using namespace std;

void AlgoCollection::Add(ExternalHasher * algo) {
	push_back(algo);
}

ExternalHasher * AlgoCollection::Find(const string& algo) {
	for (iterator i = begin(); i != end(); i++) {
		if (!strcasecmp((*i)->getName(), algo.c_str()))
			return *i;
	}
	return NULL;
}

string AlgoCollection::listOf() const {
	string s;
	for (const_iterator i = begin(); i != end(); i++) {
		if (i != begin())
			s += "|";
		s += (*i)->getName();
	}
	return s;
}

void RegisterAlgo(ExternalHasher * algo) {
	algos.Add(algo);
}

AlgoCollection algos;

