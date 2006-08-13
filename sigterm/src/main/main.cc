#include <QApplication>

#define _SDL_main_h
#include "AudioManager.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    AudioManager mgr;
    mgr.start();
    return app.exec();
}
