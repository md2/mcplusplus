#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "RNG.h"
#include "i18n.h"

RandomNumberGenerator::RandomNumberGenerator(): count(0) {
#ifdef __urandom__
	rnd = 0;
	fd = open("/dev/urandom", O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, _("open(/dev/urandom): %s\n"), strerror(errno));
		return;
	}
#else
	reset();
#endif
}

RandomNumberGenerator::~RandomNumberGenerator() {
#ifdef __urandom__
	if (fd != -1) close(fd);
#endif
}

#ifndef __urandom__
int RandomNumberGenerator::initseed() {
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, _("open(/dev/urandom): %s\n"), strerror(errno));
		return -1;
	}
	ssize_t nb = read(fd, &seed, sizeof(seed));
	close(fd);
	if (nb == -1) {
		fprintf(stderr, _("read(/dev/urandom): %s\n"), strerror(errno));
		return -1;
	}
	if (nb != sizeof(seed)) {
		fprintf(stderr, _("read(/dev/urandom): only %d bytes out of %d were read\n"), nb, sizeof(seed));
		return -1;
	}
	return 0;
}

void RandomNumberGenerator::initseed_fallback() {
	seed = getpid() + random() + time(NULL);
}

void RandomNumberGenerator::init() {
	srandom(seed);
}

void RandomNumberGenerator::reset() {
	if (initseed() == -1)
		initseed_fallback();
	init();
}
#endif

int RandomNumberGenerator::getRandom(int mod) {
	count++;
#ifdef __urandom__
	if (rnd == 0 && fd != -1) {
		read(fd, &rnd, sizeof(rnd));
	}
	int result = (rnd & 0xff) % mod;
	rnd >>= 8;
	return result;
#else
	if (count % 0x7fffffff0 == 0)
		reset();
	return random() % mod;
#endif
}

