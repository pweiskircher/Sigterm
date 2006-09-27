#define _SDL_main_h 1

#include <QApplication>
#include "MainWindow.h"
#include <QPlastiqueStyle>

#include <signal.h>
#include <SDL.h>

static void sighandler_print_msg(const char* text) {
	fprintf(stderr, "%s", text);
}

static void sighandler(int sig) {
	signal(sig, SIG_DFL);
	sighandler_print_msg("Fatal Signal received: ");
	switch(sig) {
#ifdef SIGSEGV
		case SIGSEGV:
			sighandler_print_msg("Segmentation Fault, destroying SDL and rethrowing signal\n\n");
			SDL_Quit();
			raise(SIGSEGV);
			return;
			break;
#endif
#ifdef SIGBUS
#if SIGBUS != SIGSEGV
		case SIGBUS:
			sighandler_print_msg("Bus Error");
			break;
#endif
#endif
#ifdef SIGFPE
		case SIGFPE:
			sighandler_print_msg("Floating Point Exception");
			break;
#endif
#ifdef SIGQUIT
		case SIGQUIT:
			sighandler_print_msg("Keyboard Quit");
			break;
#endif
#ifdef SIGPIPE
		case SIGPIPE:
			sighandler_print_msg("Broken Pipe");
			break;
#endif
		default:
			sighandler_print_msg("Unknown Signal");
			break;
	}
	sighandler_print_msg(" (sigterm Signal Handler)\n");
	SDL_Quit();
	exit(-sig);
}

static void setup_sighandler() {
	/* Set a handler for any fatal signal not already handled */
	int i;

	int fatal_signals[] = {
#ifdef SIGSEGV
		SIGSEGV,
#endif
#ifdef SIGBUS
		SIGBUS,
#endif
#ifdef SIGFPE
		SIGFPE,
#endif
#ifdef SIGQUIT
		SIGQUIT,
#endif
		0
	};

	for (i=0; fatal_signals[i]; ++i) {
		signal(fatal_signals[i], sighandler);
	}
	return;
}

int main(int argc, char **argv) {

	setup_sighandler();	
	
	QApplication app(argc, argv);
	QApplication::setStyle(new QPlastiqueStyle);
	MainWindow m;
	m.show();
	return app.exec();
}

