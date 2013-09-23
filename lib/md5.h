#ifndef MD5_H
#define MD5_H
#include <sys/types.h>
struct MD5Context {
	u_int32_t buf[4];
	u_int32_t bits[2];
	unsigned char in[64];
};
void MD5Init(struct MD5Context *);
void MD5Update(struct MD5Context *, unsigned char const *, unsigned);
void MD5Final(unsigned char [16], struct MD5Context *);
void MD5Transform(u_int32_t [4], u_int32_t const [16]);
#endif

