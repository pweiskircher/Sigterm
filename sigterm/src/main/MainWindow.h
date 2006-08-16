#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include "ui_mainwindow.h"
#include "AudioManager.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

    public:
	MainWindow(QWidget *parent = 0);

    private slots:
	void audioPaused(bool inPaused);

	void on_addButton_clicked();
        void on_playButton_clicked();
	void on_actionQuit_activated();
	void on_playQueue_doubleClicked(const QModelIndex &index);

    private:
	AudioManager mAudioManager;
};

#endif
