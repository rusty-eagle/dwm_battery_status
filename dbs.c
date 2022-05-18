#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/sysctl.h>
#include <X11/Xlib.h>

#include "config.h"

static Display *x_display;
static const int signals[] = {
	SIGHUP,
	SIGINT,
	SIGABRT,
	SIGTERM,
	SIGINFO,
};
static char status_msg[STATUS_MAX];

static char battery(void);
static void handler(int);

static char battery(void) {
	static int bl;
	size_t bl_sz = sizeof(bl) * 4;
	if (sysctlbyname(SYSCTL_BATTERY, &bl, &bl_sz, NULL, 0) == -1) {
		warn("sysctl failure to get battery life");
		return -1;
	}
	return (char)bl;
}

static void handler(const int signum) {
	const int save_errno = errno;

	psignal(signum, "caught signal");
	switch(signum) {
		case SIGINT:
		case SIGTERM:
			(void)XCloseDisplay(x_display);
			exit(0);
		case SIGALRM:
		case SIGHUP:
			break;
		case SIGINFO:
			if (puts(status_msg) == EOF)
				warn("cannot give status");
			break;
	}

	errno = save_errno;
}

int main(int argc, char **argv) {
	static char daemon_arg = 0, result;

	if (argc > 1) {
		if (argc == 2 && strcmp(argv[1], "-d") == 0)
			daemon_arg = 1;
		else {
			fprintf(stderr, "Usage: %s [-d]\n", getprogname());
			return 1;
		}
	}
	
	if (daemon_arg && daemon(1, 1) == -1)
		err(1, "Could not start daemon");

	for (result = 0; result < (int)(sizeof(signals) / sizeof((signals)[0])); ++result)
		if (signal(signals[result], handler) == SIG_ERR)
			err(1, "Could not assign signal");

	if ((x_display = XOpenDisplay(NULL)) == NULL)
		errx(1, "Cannot open display");

status_loop:
	if ((result = snprintf(status_msg, STATUS_MAX, STATUS_FMT, battery())) == -1)
		warn("trouble stuffing status_msg");
	else if (result >= STATUS_MAX)
		warn("status message is too long");
	else {
		(void)XStoreName(x_display, DefaultRootWindow(x_display), status_msg);
		(void)XSync(x_display, False);
	}

	(void)sleep(SLEEP_INTERVAL);

	goto status_loop;

	/* useless due to loop */
    return result;
}
