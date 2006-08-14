#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

    public:
	MainWindow(QWidget *parent = 0);
};

#endif
