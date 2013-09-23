#ifndef ALGO_MD5_H
#define ALGO_MD5_H

#ifdef USE_INTERNAL_MD5
#include "md5.h"
#elif HAVE_OPENSSL
#include <openssl/md5.h>
#endif
#include "ExternalHasher.h"
#include <map>

class MD5: public ExternalHasher {
#ifdef USE_INTERNAL_MD5
		struct MD5Context ctx;
#elif HAVE_OPENSSL
		MD5_CTX ctx;
#endif
	public:
		static const int DIGEST_LENGTH = 16;
		typedef union {
			uint8_t asUINT8[DIGEST_LENGTH];
			uint16_t asUINT16[8];
			uint32_t asUINT32[4];
		} HashType;
		typedef std::map<HashType, std::string> DC;
	private:
		DC *digests[65536];
		HashType digest;
		char hexdigest[DIGEST_LENGTH*2+1];

		const uint8_t *plainToBinary(const char *, size_t);
		const uint8_t *prToBinary(const char *);
		int selfTest();
		int testHash(const char *, const char *);
	public:
		MD5();
		~MD5();
		const char *getName() const;
		size_t getLength() const;
		bool empty() const;
		size_t size() const;
		int addEntry(const char *, const char *);
		int computeAndFind(const char *, size_t);
		int lookup(HashType *);
		const char *toPr();
		std::string getDataAndEraseFound();
};

extern class MD5 * a_md5;

#endif

