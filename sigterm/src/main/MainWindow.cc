#include "MainWindow.h"
#include "PlayQueue.h"
#include "AudioFile.h"
#include "Library.h"
#include "AudioDecoder.h"
#include "Preferences.h"
#include "LastFMClient.h"
#include "AboutWindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QShortcut>

enum PlayMode {
	PlayMode_Stopped,
	PlayMode_Playing,
	PlayMode_Paused
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), mSettings(QSettings::IniFormat, QSettings::UserScope, "SIGTERM", "sigterm") {
	QString defaultDataDirectory = QDir::homePath() + "/.sigterm";
	mDataDirectory = mSettings.value("Main/DataDirectory", defaultDataDirectory).toString();
	QDir dataDir(mDataDirectory);
	if (!dataDir.exists()) {
		if (!dataDir.mkpath(dataDir.absolutePath())) {
			qWarning("Could not create directory '%s'", qPrintable(dataDir.absolutePath()));
			exit(EXIT_FAILURE);
		}
	}

	qRegisterMetaType<quint32>("quint32");

	mPreferences = new Preferences(&mSettings, this);
	mLastFMClient = new LastFMClient(mDataDirectory + "/lastfm.ini");

	mSpaceShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
	connect(mSpaceShortcut, SIGNAL(activated()), SLOT(on_playButton_clicked()));

	mAboutWindow = NULL;


	connect(mPreferences, SIGNAL(lastFMSettingsChanged(const QString &, const QString &)),
			mLastFMClient, SLOT(usernameAndPasswordHashUpdated(const QString &, const QString &)));

	mLibrary = new Library(mDataDirectory + "/library.db");
	mLibrary->open();

	setupUi(this);
	connect(&mAudioManager,	SIGNAL(audioPaused(bool)),	SLOT(audioPaused(bool)));
	connect(qApp,			SIGNAL(lastWindowClosed()),	SLOT(on_actionQuit_activated()));

	mAudioManager.init();

	connect(&mTrackDisplayUpdater,	SIGNAL(timeout()),			SLOT(updateTrackDisplay()));
	connect(timeSlider,				SIGNAL(valueChanged(int)),	SLOT(seekSliderChangedValue(int)));
	connect(timeSlider,				SIGNAL(sliderMoved(int)),	SLOT(seekSliderMoved(int)));
	connect(timeSlider,				SIGNAL(sliderPressed()),	SLOT(seekSliderPressed()));
	connect(timeSlider,				SIGNAL(sliderReleased()),	SLOT(seekSliderReleased()));
	connect(volumeSlider,			SIGNAL(valueChanged(int)),	SLOT(volumeSliderChangedValue(int)));

	// TODO: make this work with a connect()
	mAudioManager.setVolume(mSettings.value("Main/Volume", 80).toInt());
	volumeSlider->setValue((int)((volumeSlider->maximum() * mAudioManager.volume())/100));

	playQueue->setup(mAudioManager.playQueue());

	connect(mAudioManager.playQueue(),	SIGNAL(audioFileStarted(AudioFile*)),			SLOT(audioFileStarted(AudioFile*)));
	connect(mAudioManager.playQueue(),	SIGNAL(audioFileStopped(AudioFile*, quint32)),	SLOT(audioFileStopped(AudioFile*, quint32)));
	connect(mAudioManager.playQueue(),	SIGNAL(audioFileStopped(AudioFile*, quint32)),	mLastFMClient, SLOT(trackStoppedPlaying(AudioFile*, quint32)));
	connect(deleteButton,				SIGNAL(clicked()),								SLOT(removeSelectedTracks()));
	connect(playQueue,					SIGNAL(removeSelectedTracksKeyPressed()),		SLOT(removeSelectedTracks()));

	mAudioManager.playQueue()->loadFromFile(mDataDirectory + "/PlayQueue.m3u");

	/* restore state (read everything in one rush so state doesnt get overwritten) */
	int lastTrack = mSettings.value("State/Track", 0).toInt();
	int lastPosition = mSettings.value("State/Position", 0).toInt();
	PlayMode lastMode = (PlayMode)mSettings.value("State/Mode", PlayMode_Stopped).toInt();
	bool playAutomatically = mPreferences->autoPlayEnabled();

	mAudioManager.playQueue()->setNextTrack(lastTrack);

	if (playAutomatically && lastPosition != 0) {
		qDebug() << "restoring play position " << lastPosition;
		mAudioManager.playQueue()->setStartTime(lastPosition*1000);
	}

	// for setNextTrack to work
	mAudioManager.skipTrack();

	if (playAutomatically && lastMode == PlayMode_Playing)
		mAudioManager.setPause(false);

	mSeekSliderUserUpdate = false;

	if (mSettings.contains("MainWindow/Position")) {
		QPoint pos = mSettings.value("MainWindow/Position").toPoint();
		if (pos.isNull() == false)
			move(pos);
	}

	if (mSettings.contains("MainWindow/Size")) {
		QSize size = mSettings.value("MainWindow/Size").toSize();
		if (size.isNull() == false)
			resize(size);
	}

	mPreferences->init();
}

MainWindow::~MainWindow() {
	mAudioManager.playQueue()->saveToFile(mDataDirectory + "/PlayQueue.m3u");
	delete mLibrary;
}

void MainWindow::audioPaused(bool inPause) {
	if (inPause) {
		playButton->setText("Play");
		mTrackDisplayUpdater.stop();
	} else {
		playButton->setText("Pause");
		mTrackDisplayUpdater.start(100);
	}
}

void MainWindow::updateTrackDisplay() {
	AudioFile *af = mAudioManager.playQueue()->playingTrack();
	quint32 timeTotalValue, timePlayedValue;
	if (!af) {
		timeTotalValue = timePlayedValue = 0;
	} else {
		timeTotalValue = af->timeTotal();
		timePlayedValue = af->timePlayed();
	}

	timeSlider->setMinimum(0);
	timeSlider->setMaximum(timeTotalValue);

	if (mSeekSliderUserUpdate == false)
		timeSlider->setValue(timePlayedValue);

	mSettings.setValue("State/Position", timePlayedValue);
}

void MainWindow::seekSliderChangedValue(int inValue) {
	QSlider *slider = (QSlider *)QObject::sender();

	quint32 timeTotalValue, timePlayedValue;
	QString help;

	timeTotalValue = slider->maximum();
	timePlayedValue = inValue;

	help.sprintf("%d:%02d", timePlayedValue/60, timePlayedValue%60);
	timePlayed->setText(help);

	quint32 left;
	if ((int)(timeTotalValue - timePlayedValue) < 0)
		left = 0;
	else
		left = timeTotalValue - timePlayedValue;
	help.sprintf("-%d:%02d", left/60, left%60);
	timeLeft->setText(help);
}

void MainWindow::seekSliderMoved(int inValue) {
	mSeekSliderUserUpdateValue = inValue;
}

void MainWindow::seekSliderPressed() {
	mSeekSliderUserUpdate = true;
}

void MainWindow::seekSliderReleased() {
	/* TODO: seekToTime should probably be in class AudioManager (or class PlayQueue?) */
	AudioFile *af = mAudioManager.playQueue()->playingTrack();
	if (af) {
		af->seekToTime(mSeekSliderUserUpdateValue*1000);
		qDebug("seekSliderUserUpdateValue %d", mSeekSliderUserUpdateValue);
	}

	mSeekSliderUserUpdate = false;
}

void MainWindow::volumeSliderChangedValue(int inValue) {
	QSlider *slider = (QSlider *)QObject::sender();

	int volume = (inValue*100)/slider->maximum();
	mAudioManager.setVolume(volume);
	
	mSettings.setValue("Main/Volume", volume);
}

void MainWindow::on_nextButton_clicked() {
	mAudioManager.nextTrack();
}

void MainWindow::on_playButton_clicked() {
	if (mAudioManager.paused())
		mSettings.setValue("State/Mode", PlayMode_Playing);
	else
		mSettings.setValue("State/Mode", PlayMode_Paused);

	mAudioManager.togglePause();
}

void MainWindow::on_prevButton_clicked() {
	mAudioManager.prevTrack();
}

void MainWindow::on_addButton_clicked() {
	QStringList fileFilter = mAudioManager.supportedFileFilter();
	fileFilter.move(fileFilter.size()-1, 0);

	QString directory = mSettings.value("OpenDialog/LastBrowsedDirectory", QDir::homePath()).toString();

	QStringList files = QFileDialog::getOpenFileNames(this, "Add Music Files ...", directory,
			fileFilter.join(";;"));

	qSort(files.begin(), files.end());
	for (int i=0; i<files.size(); i++) {
		AudioFile *af = new AudioFile(files[i], &mAudioManager);
		af->addToQueue();

		// ugly workaround because we don't get the last directory the user browsed ..
		// but I'm pretty sure that the file *is* in the directory the user browsed last ;)
		if (i == 0) {
			QFileInfo fi(files[i]);
			mSettings.setValue("OpenDialog/LastBrowsedDirectory", fi.absolutePath());
		}
	}
}

void MainWindow::removeSelectedTracks() {
	QModelIndexList l = playQueue->selectionModel()->selectedIndexes();
	mAudioManager.playQueue()->removeTracks(l);
}

void MainWindow::on_addPlaylistButton_clicked() {

	QString directory = mSettings.value("OpenDialog/LastBrowsedDirectory", QDir::homePath()).toString();

	QString file = QFileDialog::getOpenFileName(this, "Load Playlist ...", directory, "Playlist Files (*.m3u)");
	if (file.isEmpty())
		return;

	mAudioManager.playQueue()->appendFromFile(file);
	
	// same ugly workaround as above
	QFileInfo fi(file);
	mSettings.setValue("OpenDialog/LastBrowsedDirectory", fi.absolutePath());
}

void MainWindow::on_actionPreferences_activated() {
	mPreferences->exec();
}

void MainWindow::on_actionQuit_activated() {
	mSettings.setValue("MainWindow/Position", pos());
	mSettings.setValue("MainWindow/Size", size());

	mAudioManager.quit();
	qApp->quit();
}

void MainWindow::on_actionLast_FM_activated() {
	mLastFMClient->showDialog();
}

void MainWindow::on_actionPlayer_activated() {
	activateWindow();
	raise();
}

void MainWindow::on_actionAbout_activated() {
	if (!mAboutWindow)
		mAboutWindow = new AboutWindow(this);

	if (mAboutWindow) {
		mAboutWindow->show();
		mAboutWindow->activateWindow();
		mAboutWindow->raise();
	}
}

void MainWindow::on_playQueue_doubleClicked(const QModelIndex &index) {
	mAudioManager.playQueue()->setNextTrack(index.row());

	mAudioManager.skipTrack();
	if (mAudioManager.paused()) {
		mAudioManager.audioStorage()->clear();
		mAudioManager.togglePause();
	}
}

void MainWindow::audioFileStarted(AudioFile *inAudioFile) {
	qDebug() << "Started Playing: " << inAudioFile->filePath() << "samples played: " << inAudioFile->playedSamples();
	mSettings.setValue("State/Mode", PlayMode_Playing);
	mSettings.setValue("State/Track", mAudioManager.playQueue()->currentFileId());
}

void MainWindow::audioFileStopped(AudioFile *inAudioFile, quint32 inTimePlayed) {
	qDebug() << "Stopped Playing: " << inAudioFile->filePath() << "samples played: " << inAudioFile->playedSamples();
	mSettings.setValue("State/Mode", PlayMode_Stopped);
	mSettings.setValue("State/Track", mAudioManager.playQueue()->currentFileId());
}

