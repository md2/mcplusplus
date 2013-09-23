#ifndef RNG_H
#define RNG_H

#undef __urandom__

class RandomNumberGenerator {
		unsigned long long count;
#ifdef __urandom__
		int fd;
		unsigned int rnd;
#else
		unsigned int seed;
		int initseed();
		void initseed_fallback();
		void init();
		void reset();
#endif
	public:
		RandomNumberGenerator();
		~RandomNumberGenerator();
		int getRandom(int mod);
};

#endif

