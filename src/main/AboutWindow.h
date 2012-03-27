#ifndef _ABOUTWINDOW_H_
#define _ABOUTWINDOW_H

#include "ui_aboutwindow.h"
#include <QDialog>

class AboutWindow : public QDialog, private Ui::AboutWindow {
	Q_OBJECT

	public:
		AboutWindow(QWidget *parent = 0);
		~AboutWindow();
};

#endif

