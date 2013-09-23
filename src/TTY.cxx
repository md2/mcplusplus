#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "common.h"

TTY::TTY() {
	struct termios ti;
	if ((fd = open("/dev/tty", O_RDONLY | O_NONBLOCK)) < 0) {
		fprintf(stderr, _("open(/dev/tty): %s\n"), strerror(errno));
		return;
	}
	tcgetattr(fd, &ti);
	saved_ti = ti;
	ti.c_lflag &= ~(ICANON | ECHO);
	ti.c_cc[VMIN] = 1;
	ti.c_cc[VTIME] = 0;
	tcsetattr(fd, TCSANOW, &ti);
}

TTY::~TTY() {
	tcsetattr(fd, TCSANOW, &saved_ti);
	close(fd);
}

int TTY::ReadKey() {
	int c;
	if (read(fd, &c, 1) > 0)
		return c;
	return -1;
}

TTY tty;

