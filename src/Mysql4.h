#ifndef ALGO_MYSQL4_H
#define ALGO_MYSQL4_H

#include "ExternalHashFunction.h"

class Mysql4: public ExternalHashFunction {
		static const int DIGEST_LENGTH = 8;
		uint8_t binary[DIGEST_LENGTH];
		char base16[DIGEST_LENGTH*2+1];
		std::string binary2;
		int PerformSelfTests();
		int testhash(const char *, const char *);
	public:
		Mysql4();
		const char *getName() const;
		size_t getLength() const;
		const uint8_t *PlaintextToBinary(const char *, size_t);
		const char *BinaryToPrintable(const uint8_t *);
		const uint8_t *PrintableToBinary(const char *);
		std::string& ToString(const uint8_t *);
};

extern class Mysql4 * a_mysql4;

#endif

