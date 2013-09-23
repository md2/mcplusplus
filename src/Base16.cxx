#include <stdio.h>
#include "Base16.h"
#include "HelperFunctions.h"

void base16_encode(char *dst, const uint8_t *src, size_t len) {
	char *p = dst;
	for (int i = 0; i < len; i++) {
		snprintf(p, 3, "%02x", (int)src[i]);
		p += 2;
	}
	dst[len*2] = '\0';
}

int base16_decode(uint8_t *dst, const char *src, size_t len) {
	for (int j = 0, i = 0; j < len; i += 2, j++) {
		dst[j] = (hex(src[i]) & 0x0f) << 4;
		dst[j] += hex(src[i+1]) & 0x0f;
	}
	return 0;
}

