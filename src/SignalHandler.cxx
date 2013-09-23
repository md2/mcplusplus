#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include "common.h"

void sig_handler(int);
SignalHandler *ha = NULL;

SignalHandler::SignalHandler() {
	assert(ha == NULL);
	ha = this;
	interrupted = false;
	ticks = 0;
	struct sigaction sa;
	struct itimerval it;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = sig_handler;
	sigaction(SIGALRM, &sa, NULL);
	it.it_value.tv_sec = 1;
	it.it_value.tv_usec = 0;
	it.it_interval = it.it_value;
	if (setitimer(ITIMER_REAL, &it, NULL) != 0) {
		fprintf(stderr, _("setitimer: %s\n"), strerror(errno));
		return;
	}
	signal(SIGINT, sig_handler);
}

SignalHandler::~SignalHandler() {
	struct itimerval it;
	memset(&it, 0, sizeof(it));
	setitimer(ITIMER_REAL, &it, NULL);
	signal(SIGALRM, SIG_DFL);
	signal(SIGINT, SIG_DFL);
}

void SignalHandler::OnEvent(EventType e) {
	switch (e) {
		case E_TIMER:
			OnTimer();
			break;
		case E_INTERRUPT:
			OnInterrupt();
			break;
		default:
			break;
	}
}

void SignalHandler::OnTimer() {
	ticks++;
}

void SignalHandler::OnInterrupt() {
	interrupted = true;
}

bool SignalHandler::Interrupted() const {
	return interrupted;
}

long long int SignalHandler::Ticks() const {
	return ticks;
}

void sig_handler(int signum) {
	if (ha)
		if (signum == SIGALRM)
			ha->OnEvent(SignalHandler::E_TIMER);
		else if (signum == SIGINT)
			ha->OnEvent(SignalHandler::E_INTERRUPT);
}

