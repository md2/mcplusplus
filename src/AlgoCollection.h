#ifndef ALGOCOLLECTION_H
#define ALGOCOLLECTION_H

#include <string>
#include <list>
#include "ExternalHasher.h"

class AlgoCollection: public std::list<ExternalHasher *> {
	public:
		void Add(ExternalHasher *);
		ExternalHasher * Find(const std::string&);
		std::string listOf() const;
};

extern AlgoCollection algos;
void RegisterAlgo(ExternalHasher *);

#endif

