#define _SDL_main_h 1

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MainWindow m;
    m.show();
    return app.exec();
}

