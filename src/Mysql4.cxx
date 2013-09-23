#include <string.h>
#include <assert.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <byteswap.h>
#include "AlgoCollection.h"
#include "Mysql4.h"
#include "Base16.h"

using namespace std;

Mysql4::Mysql4() {
	int i;
	if ((i = PerformSelfTests()) == 0)
		RegisterAlgo(this);
	else
		cerr << getName() << ": " << i << " test(s) failed."
		     << endl;
}

const char *Mysql4::getName() const {
	return "MYSQL4";
}

size_t Mysql4::getLength() const {
	return DIGEST_LENGTH;
}

const uint8_t *Mysql4::PlaintextToBinary(const char *plaintext, size_t len) {
	register uint32_t nr = 1345345333L, add = 7, nr2 = 0x12345671L;
	uint32_t tmp;
	uint32_t *result = (uint32_t *)binary;
	for (; *plaintext ; plaintext++) {
		if (*plaintext == ' ' || *plaintext == '\t')
			continue;
		tmp = *(unsigned char *)plaintext;
		nr ^= (((nr & 63) + add) * tmp) + (nr << 8);
		nr2 += (nr2 << 8) ^ nr;
		add += tmp;
	}
	result[0] = bswap_32(nr & (((uint32_t) 1L << 31) - 1L));
	result[1] = bswap_32(nr2 & (((uint32_t) 1L << 31) - 1L));
	return binary;
}

const char *Mysql4::BinaryToPrintable(const uint8_t *binary) {
	base16_encode(base16, binary, DIGEST_LENGTH);
	return base16;
}

const uint8_t *Mysql4::PrintableToBinary(const char *base16) {
	if (strlen(base16) != 2*DIGEST_LENGTH)
		return (uint8_t *)NULL;
	base16_decode(binary, base16, DIGEST_LENGTH);
	return binary;
}

string& Mysql4::ToString(const uint8_t *binary) {
	binary2 = string((char *)binary, DIGEST_LENGTH);
	return binary2;
}

int Mysql4::PerformSelfTests() {
	int i = 0;
	i += testhash("111", "773214e90eb99bdb");
	i += testhash("mysql", "68d4f47c49a579c9");
	return i;
}

int Mysql4::testhash(const char *plaintext, const char *hexdigest) {
	PlaintextToBinary(plaintext, strlen(plaintext));
	BinaryToPrintable(binary);
	return strcmp(base16, hexdigest) ? 1 : 0;
}

class Mysql4 * a_mysql4 = NULL;

