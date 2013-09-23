#ifndef TTY_H
#define TTY_H

#include <termios.h>

class TTY {
		int fd;
		struct termios saved_ti;
	public:
		TTY();
		~TTY();
		int ReadKey();
};

extern TTY tty;

#endif

