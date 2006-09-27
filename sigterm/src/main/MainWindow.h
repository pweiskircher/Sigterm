#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include "ui_mainwindow.h"
#include "AudioManager.h"

#include <QSettings>
#include <QTimer>

class Library;
class Preferences;
class LastFMClient;
class AudioFile;

class MainWindow : public QMainWindow, private Ui::MainWindow {
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();

	private slots:
		void audioPaused(bool inPaused);
		void updateTrackDisplay();

		void seekSliderChangedValue(int inValue);
		void seekSliderMoved(int inValue);
		void seekSliderPressed();
		void seekSliderReleased();

		void volumeSliderChangedValue(int inValue);
		
		void on_nextButton_clicked();
		void on_playButton_clicked();
		void on_prevButton_clicked();

		void on_addButton_clicked();
		void on_addPlaylistButton_clicked();

		void on_actionPreferences_activated();
		void on_actionQuit_activated();
		void on_playQueue_doubleClicked(const QModelIndex &index);

		void audioFileStarted(AudioFile *inAudioFile);
		void audioFileStopped(AudioFile *inAudioFile, quint32 inTimePlayed);

		void removeSelectedTracks();
		
	private:
		AudioManager mAudioManager;
		QSettings mSettings;
		QTimer mTrackDisplayUpdater;
		Library *mLibrary;
		Preferences *mPreferences;
		LastFMClient *mLastFMClient;
		QString mDataDirectory;

		bool mSeekSliderUserUpdate;
		quint32 mSeekSliderUserUpdateValue;
};

#endif
