#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include "ui_mainwindow.h"
#include "AudioManager.h"

#include <QTimer>

class MainWindow : public QMainWindow, private Ui::MainWindow {
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);

	private slots:
		void audioPaused(bool inPaused);
	    void updateTrackDisplay();

		void seekSliderChangedValue(int inValue);
		void seekSliderMoved(int inValue);
		void seekSliderPressed();
		void seekSliderReleased();

		void on_nextButton_clicked();
		void on_playButton_clicked();
		void on_prevButton_clicked();

		void on_addButton_clicked();
		void on_deleteButton_clicked();

		void on_actionQuit_activated();
		void on_playQueue_doubleClicked(const QModelIndex &index);

	private:
		AudioManager mAudioManager;
		QTimer mTrackDisplayUpdater;

		bool mSeekSliderUserUpdate;
		quint32 mSeekSliderUserUpdateValue;
};

#endif
