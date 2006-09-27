#include "MainWindow.h"
#include "PlayQueue.h"
#include "AudioFile.h"
#include "Library.h"
#include "AudioDecoder.h"
#include "Preferences.h"
#include "LastFMClient.h"

#include <QDebug>
#include <QFileDialog>
#include <QHeaderView>

enum PlayMode {
	PlayMode_Stopped,
	PlayMode_Playing,
	PlayMode_Paused
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), mSettings(QSettings::IniFormat, QSettings::UserScope, "SIGTERM", "sigterm") {

	mSettings.setValue("Misc/Dummy", "IgnoreMe");
	mSettings.sync();

	QString defaultDataDirectory = QDir::homePath() + "/.sigterm";
	mDataDirectory = mSettings.value("Main/DataDirectory", defaultDataDirectory).toString();
	QDir dataDir(mDataDirectory);
	if (!dataDir.exists()) {
		if (!dataDir.mkpath(dataDir.absolutePath())) {
			qWarning("Could not create directory '%s'", qPrintable(dataDir.absolutePath()));
			exit(EXIT_FAILURE);
		}
	}

	mPreferences = new Preferences(this);

	mLibrary = new Library(mDataDirectory + "/library.db");
	mLibrary->open();

	mLastFMClient = new LastFMClient(mDataDirectory + "/lastfm.ini");
	
	setupUi(this);
	connect(&mAudioManager, SIGNAL(audioPaused(bool)), SLOT(audioPaused(bool)));
	connect(qApp, SIGNAL(lastWindowClosed()), SLOT(on_actionQuit_activated()));
	mAudioManager.init();

	connect(&mTrackDisplayUpdater, SIGNAL(timeout()), SLOT(updateTrackDisplay()));
	connect(timeSlider, SIGNAL(valueChanged(int)), SLOT(seekSliderChangedValue(int)));
	connect(timeSlider, SIGNAL(sliderMoved(int)), SLOT(seekSliderMoved(int)));
	connect(timeSlider, SIGNAL(sliderPressed()), SLOT(seekSliderPressed()));
	connect(timeSlider, SIGNAL(sliderReleased()), SLOT(seekSliderReleased()));

	connect(volumeSlider, SIGNAL(valueChanged(int)), SLOT(volumeSliderChangedValue(int)));
	mAudioManager.setVolume(mSettings.value("Main/Volume", 80).toInt());
	volumeSlider->setValue((int)((volumeSlider->maximum() * mAudioManager.volume())/100));
	
	playQueue->setModel(mAudioManager.playQueue());

	playQueue->header()->resizeSection(PlayQueue::eIsPlaying, 20);
	playQueue->header()->resizeSection(PlayQueue::eTrackNumber, 25);
	playQueue->header()->resizeSection(PlayQueue::eArtist, 100);
	playQueue->header()->resizeSection(PlayQueue::eAlbum, 100);
	playQueue->header()->resizeSection(PlayQueue::eTitle, 150);
	playQueue->header()->setResizeMode(PlayQueue::eTitle, QHeaderView::Stretch);
	playQueue->header()->resizeSection(PlayQueue::eTotalTime, 20);

	playQueue->header()->setStretchLastSection(false);
	playQueue->setSelectionMode(QAbstractItemView::ExtendedSelection);
	playQueue->setDragEnabled(true);
	playQueue->setAcceptDrops(true);
	playQueue->setDropIndicatorShown(true);

	connect(mAudioManager.playQueue(), SIGNAL(audioFileStarted(AudioFile*)), SLOT(audioFileStarted(AudioFile*)));
	connect(mAudioManager.playQueue(), SIGNAL(audioFileStopped(AudioFile*)), SLOT(audioFileStopped(AudioFile*)));
	
	connect(deleteButton, SIGNAL(clicked()), SLOT(removeSelectedTracks()));
	connect(playQueue, SIGNAL(removeSelectedTracksKeyPressed()), SLOT(removeSelectedTracks()));

	mAudioManager.playQueue()->loadFromFile(mDataDirectory + "/PlayQueue.m3u");

	/* restore state */
	int lastTrack = mSettings.value("State/Track", 0).toInt();
	mAudioManager.playQueue()->setNextTrack(lastTrack);
	mAudioManager.skipTrack();

	PlayMode lastMode = (PlayMode)mSettings.value("State/Mode", PlayMode_Stopped).toInt();
	bool playAutomatically = mSettings.value("Main/StatePlayAutomatically", false).toBool();
	if (playAutomatically && lastMode == PlayMode_Playing)
		mAudioManager.setPause(false);

#if 0
	/* XXX race condition: this here works only if PlayQueue already received the audioFileStartedPlaying signal, which is of course: TEHSUX */	
	int lastPosition = mSettings.value("State/Position", 0).toInt();
	if (lastPosition != 0) {
		qDebug() << "restoring play position " << lastPosition;
		AudioFile *af = mAudioManager.playQueue()->playingTrack();
		if (af)
			af->seekToTime(lastPosition);
	}
#endif
	
	mSeekSliderUserUpdate = false;
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
	mAudioManager.quit();
	qApp->quit();
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

void MainWindow::audioFileStopped(AudioFile *inAudioFile) {
	qDebug() << "Stopped Playing: " << inAudioFile->filePath() << "samples played: " << inAudioFile->playedSamples();
	mSettings.setValue("State/Mode", PlayMode_Stopped);
	mSettings.setValue("State/Track", mAudioManager.playQueue()->currentFileId());
}

