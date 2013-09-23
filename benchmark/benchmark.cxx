#if HAVE_BEECRYPT
#include <beecrypt/sha1.h>
#include <beecrypt/md5.h>
#include <beecrypt/sha256.h>
#endif
#if HAVE_GCRYPT
#include <gcrypt.h>
#endif
#if HAVE_MHASH
#include <mhash.h>
#endif
#if HAVE_OPENSSL
#include <openssl/sha.h>
#include <openssl/md5.h>
#endif
#if HAVE_CRYPTOPP
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <crypto++/md5.h>
#include <crypto++/sha.h>
#endif
#if HAVE_BOTAN
#include <botan/md5.h>
#endif
#if HAVE_GNUTLS
#include <gnutls/gnutls.h>
#endif
#include "md5.h"

#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#define START_BENCHMARK(x, y) \
	printf("%15s     %-10s", x, y); \
	gettimeofday(&t0, NULL); \
	for (i=0; i<1000000L; i++) {

#define STOP_BENCHMARK \
	} \
	gettimeofday(&t1, NULL); \
	lapse = (t1.tv_sec-t0.tv_sec)+(t1.tv_usec-t0.tv_usec)/1000000.0; \
	printf("    %-13.5g %g\n", lapse, 1000000/lapse);

main() {
	long i;
	double lapse;
	struct timeval t0, t1;
	unsigned char b[256];
	const char * plaintext = "johntheripper";
	size_t len = strlen(plaintext);

	printf("\n"); /* MD5 */

	printf(" MESSAGE DIGEST     LIBRARY       TIME (s)      SPEED (p/s)    \n");
	printf(" --------------------------------------------------------------\n");

	{
		struct MD5Context ctx;
		START_BENCHMARK("MD5", "md5 (int.)")
		MD5Init(&ctx);
		MD5Update(&ctx, (const unsigned char *)plaintext, len);
		MD5Final(b, &ctx);
		STOP_BENCHMARK
	}

#if HAVE_CRYPTOPP
	{
		CryptoPP::Weak1::MD5 md5;
		START_BENCHMARK("MD5", "crypto++")
		md5.CalculateDigest(b, (const byte *)plaintext, len);
		STOP_BENCHMARK
	}
#endif

#if HAVE_GNUTLS
	{
		gnutls_datum d = { (unsigned char *)plaintext, len };
		size_t l;
		START_BENCHMARK("MD5", "gnutls")
		l = sizeof(b) - 1;
		gnutls_fingerprint(GNUTLS_DIG_MD5, &d, b, &l);
		STOP_BENCHMARK
	}
#endif

#if HAVE_BOTAN
	{
		START_BENCHMARK("MD5", "botan")
		Botan::MD5 md;
		md.update((byte*)plaintext, len);
		md.final(b);
		STOP_BENCHMARK
	}
#endif

#if HAVE_BEECRYPT
	{
		md5Param par;
		START_BENCHMARK("MD5", "beecrypt")
		md5Reset(&par);
		md5Update(&par, (const byte *)plaintext, len);
		md5Digest(&par, b);
		STOP_BENCHMARK
	}
#endif

#if HAVE_GCRYPT
	{
		START_BENCHMARK("MD5", "gcrypt")
		gcry_md_hash_buffer(GCRY_MD_MD5, b, plaintext, len);
		STOP_BENCHMARK
	}
#endif

#if HAVE_MHASH
	{
		MHASH td;
		START_BENCHMARK("MD5", "mhash")
		td = mhash_init(MHASH_MD5);
		mhash(td, plaintext, len);
		mhash_deinit(td, b);
		STOP_BENCHMARK
	}
#endif

#if HAVE_OPENSSL
	{
		START_BENCHMARK("MD5", "openssl")
		MD5((const unsigned char *)plaintext, len, b);
		STOP_BENCHMARK
	}
#endif

	printf("\n"); /* SHA1 */

#if HAVE_CRYPTOPP
	{
		CryptoPP::SHA1 sha1;
		START_BENCHMARK("SHA1", "crypto++")
		sha1.CalculateDigest(b, (const byte *)plaintext, len);
		STOP_BENCHMARK
	}
#endif

#if HAVE_BEECRYPT
	{
		sha1Param par;
		START_BENCHMARK("SHA1", "beecrypt")
		sha1Reset(&par);
		sha1Update(&par, (const byte *)plaintext, len);
		sha1Digest(&par, b);
		STOP_BENCHMARK
	}
#endif

#if HAVE_GCRYPT
	{
		START_BENCHMARK("SHA1", "gcrypt")
		gcry_md_hash_buffer(GCRY_MD_SHA1, b, plaintext, len);
		STOP_BENCHMARK
	}
#endif

#if HAVE_MHASH
	{
		MHASH td;
		START_BENCHMARK("SHA1", "mhash")
		td = mhash_init(MHASH_SHA1);
		mhash(td, plaintext, len);
		mhash_deinit(td, b);
		STOP_BENCHMARK
	}
#endif

#if HAVE_OPENSSL
	{
		START_BENCHMARK("SHA1", "openssl")
		SHA1((const unsigned char *)plaintext, len, b);
		STOP_BENCHMARK
	}
#endif

	printf("\n"); /* SHA256 */

#if HAVE_CRYPTOPP
	{
		CryptoPP::SHA256 sha256;
		START_BENCHMARK("SHA256", "crypto++")
		sha256.CalculateDigest(b, (const byte *)plaintext, len);
		STOP_BENCHMARK
	}
#endif

#if HAVE_BEECRYPT
	{
		sha256Param par;
		START_BENCHMARK("SHA256", "beecrypt")
		sha256Reset(&par);
		sha256Update(&par, (const byte *)plaintext, len);
		sha256Digest(&par, b);
		STOP_BENCHMARK
	}
#endif

#if HAVE_GCRYPT
	{
		START_BENCHMARK("SHA256", "gcrypt")
		gcry_md_hash_buffer(GCRY_MD_SHA256, b, plaintext, len);
		STOP_BENCHMARK
	}
#endif

#if HAVE_MHASH
	{
		MHASH td;
		START_BENCHMARK("SHA256", "mhash")
		td = mhash_init(MHASH_SHA256);
		mhash(td, plaintext, len);
		mhash_deinit(td, b);
		STOP_BENCHMARK
	}
#endif

#if HAVE_OPENSSL
	{
		START_BENCHMARK("SHA256", "openssl")
		SHA256((const unsigned char *)plaintext, len, b);
		STOP_BENCHMARK
	}
#endif

	return 0;
}
