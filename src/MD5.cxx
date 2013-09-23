#include <string.h>
#include <assert.h>
#include <string>
#include "common.h"

using namespace std;

int operator<(const MD5::HashType &h1, const MD5::HashType &h2) {
	return memcmp(&h1, &h2, sizeof(MD5::HashType)) < 0;
}

MD5::MD5() {
	memset(digests, 0, sizeof(digests));
	int i;
	if ((i = selfTest()) == 0)
		RegisterAlgo(this);
	else
		fprintf(stderr, _("%s: %d test(s) failed.\n"), getName(), i);
}

MD5::~MD5() {
	for (int i = 0; i < sizeof(digests)/sizeof(digests[0]); i++)
		if (digests[i])
			delete digests[i];
}

const char *MD5::getName() const {
	return "MD5";
}

size_t MD5::getLength() const {
	return DIGEST_LENGTH;
}

const uint8_t *MD5::plainToBinary(const char *plain, size_t len) {
#ifdef USE_INTERNAL_MD5
	MD5Init(&ctx);
	MD5Update(&ctx, (const unsigned char *)plain, len);
	MD5Final((unsigned char *)&digest, &ctx);
#elif HAVE_OPENSSL
	MD5_Init(&ctx);
	MD5_Update(&ctx, plain, len);
	MD5_Final(digest.asUINT8, &ctx);
#endif
	return digest.asUINT8;
}

const char *MD5::toPr() {
	base16_encode(hexdigest, digest.asUINT8, DIGEST_LENGTH);
	return hexdigest;
}

const uint8_t *MD5::prToBinary(const char *hexdigest) {
	if (strlen(hexdigest) != 2*DIGEST_LENGTH)
		return (uint8_t *)NULL;
	base16_decode(digest.asUINT8, hexdigest, DIGEST_LENGTH);
	return digest.asUINT8;
}

int MD5::addEntry(const char *hexdigest, const char *data) {
	if (prToBinary(hexdigest) == NULL)
		return -1;
	DC *dc = digests[digest.asUINT16[0]];
	if (!dc) {
		dc = digests[digest.asUINT16[0]] = new DC;
		dc->insert(DC::value_type(digest, data));
	} else {
		DC::iterator i = dc->find(digest);
		if (i == dc->end())
			dc->insert(DC::value_type(digest, data));
		else {
			i->second += ",";
			i->second += data;
		}
	}
	return 0;
}

int MD5::computeAndFind(const char *plain, size_t len) {
	plainToBinary(plain, len);
	DC *dc = digests[digest.asUINT16[0]];
	if (!dc)
		return 0;
	DC::iterator i = dc->find(digest);
	if (i == dc->end())
		return 0;
	return 1;
}

int MD5::lookup(HashType *digest) {
	DC *dc = digests[digest->asUINT16[0]];
	if (!dc)
		return 0;
	DC::iterator i = dc->find(*digest);
	if (i == dc->end())
		return 0;
	return 1;
}

string MD5::getDataAndEraseFound() {
	string s;
	DC *dc = digests[digest.asUINT16[0]];
	if (dc) {
		DC::iterator i = dc->find(digest);
		if (i != dc->end()) {
			s = i->second;
			dc->erase(i);
			if (dc->empty()) {
				delete dc;
				digests[digest.asUINT16[0]] = NULL;
			}
		}
	}
	return s;
}

bool MD5::empty() const {
	for (int i = 0; i < sizeof(digests)/sizeof(digests[0]); i++)
		if (digests[i])
			return false;
	return true;
}

size_t MD5::size() const {
	size_t size = 0;
	for (int i = 0; i < sizeof(digests)/sizeof(digests[0]); i++)
		if (digests[i])
			size += digests[i]->size();
	return size;
}

int MD5::selfTest() {
	int i = 0;
	i += testHash("123", "202cb962ac59075b964b07152d234b70");
	i += testHash("http://lib.rus.ec", "7a70d3866524c4e70e701f4cf82fbc9c");
	i += testHash("password", "5f4dcc3b5aa765d61d8327deb882cf99");
	i += testHash("", "d41d8cd98f00b204e9800998ecf8427e");
	return i;
}

int MD5::testHash(const char *plain, const char *hexdigest) {
	plainToBinary(plain, strlen(plain));
	int ret = strcmp(toPr(), hexdigest) ? 1 : 0;
	if (ret) {
		fprintf(stderr, "result: %s should be: %s\n", toPr(), hexdigest);
	}
	return ret;
}

class MD5 * a_md5 = NULL;

