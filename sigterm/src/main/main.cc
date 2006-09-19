#define _SDL_main_h 1

#include <QApplication>
#include "MainWindow.h"
#include <QPlastiqueStyle>

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	QApplication::setStyle(new QPlastiqueStyle);
	MainWindow m;
	m.show();
	return app.exec();
}

