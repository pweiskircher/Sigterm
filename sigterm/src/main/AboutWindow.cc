#include "AboutWindow.h"
#include "svnversion.h"

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent) {
	setWindowFlags(Qt::Dialog | Qt::WindowSystemMenuHint);
	setupUi(this);

	detailInfo->setText("Version 0.1 Revision " SIGTERM_VERSION_SVNREVISION "\n"
			"\n"
			"Copyright 2006 Patrik Weiskircher\n"
			"Copyright 2006 Christian Hofstaedtler\n"
			"\n"
			"Released unter GNU General Public License v2.\n"
			);

	homepage->setUrl("http://music.sigterm.eu/");
}

AboutWindow::~AboutWindow() {
}

