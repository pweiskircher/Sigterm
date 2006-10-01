#include "AboutWindow.h"

AboutWindow::AboutWindow(QWidget *parent) : QDialog(parent) {
	setupUi(this);

	detailInfo->setText("Version 0.1+svn99+autoXXX\n"
			"\n"
			"Copyright 2006 Patrik Weiskircher\n"
			"Copyright 2006 Christian Hofstaedtler\n"
			"\n"
			"Released unter GNU General Public License v2.\n"
			"\n"
			"#sigterm FTW!\n"
			);

//homepage->setText("<a href=\"http://music.sigterm.eu/\">http://music.sigterm.eu/</a>");
	homepage->setUrl("http://music.sigterm.eu/");
}

AboutWindow::~AboutWindow() {
}
/*
void AboutWindow::on_homepage_clicked() {
	qDebug("hi!");
}
*/
