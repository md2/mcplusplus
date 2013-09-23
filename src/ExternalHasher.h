#ifndef EXTERNALHASHER_H
#define EXTERNALHASHER_H

#include <string>

class ExternalHasher {
	public:
		virtual const char *getName() const = 0;
		virtual size_t getLength() const = 0;
		virtual bool empty() const = 0;
		virtual size_t size() const = 0;
		virtual int addEntry(const char *, const char *) = 0;
		virtual int computeAndFind(const char *, size_t) = 0;
		virtual const char *toPr() = 0;
		virtual std::string getDataAndEraseFound() = 0;
};

extern ExternalHasher * hf;

#endif

