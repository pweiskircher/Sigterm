#define _SDL_main_h 1

#include <QApplication>
#include "MainWindow.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {
	int argc = 0;
	char **argv = NULL;
#else
int main(int argc, char **argv) {
#endif
    QApplication app(argc, argv);
    MainWindow m;
    m.show();
    return app.exec();
}
