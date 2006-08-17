#include "MainWindow.h"
#include "PlayQueue.h"
#include "AudioFile.h"

#include <QFileDialog>
#include <QDebug>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    connect(&mAudioManager, SIGNAL(audioPaused(bool)), SLOT(audioPaused(bool)));
    connect(qApp, SIGNAL(lastWindowClosed()), SLOT(on_actionQuit_activated()));
    mAudioManager.init();

    playQueue->setModel(mAudioManager.playQueue());

    playQueue->header()->resizeSection(PlayQueue::eIsPlaying, 20);
    playQueue->header()->resizeSection(PlayQueue::eTrackNumber, 25);
    playQueue->header()->resizeSection(PlayQueue::eArtist, 100);
    playQueue->header()->resizeSection(PlayQueue::eAlbum, 100);
    playQueue->header()->resizeSection(PlayQueue::eTitle, 150);
    playQueue->header()->setResizeMode(PlayQueue::eTitle, QHeaderView::Stretch);
    playQueue->header()->resizeSection(PlayQueue::eTotalTime, 20);

    playQueue->header()->setStretchLastSection(false);
}

void MainWindow::audioPaused(bool inPause) {
    if (inPause) {
	playButton->setText("Play");
    } else {
	playButton->setText("Pause");
    }
}


void MainWindow::on_nextButton_clicked() {
    mAudioManager.nextTrack();
}

void MainWindow::on_playButton_clicked() {
    mAudioManager.togglePause();
}

void MainWindow::on_prevButton_clicked() {
    mAudioManager.prevTrack();
}

void MainWindow::on_addButton_clicked() {
    // TODO: save last directory
    QStringList files = QFileDialog::getOpenFileNames(this, "Add Music Files", "/home", "(*.flac *.ogg)");
    for (int i=0; i<files.size(); i++) {
	AudioFile *af = new AudioFile(files[i], &mAudioManager);
	af->addToQueue();
    }
}

void MainWindow::on_deleteButton_clicked() {
    QModelIndexList l = playQueue->selectionModel()->selectedIndexes();
    mAudioManager.playQueue()->removeTracks(l);
}


void MainWindow::on_actionQuit_activated() {
    mAudioManager.quit();
    qApp->quit();
}


void MainWindow::on_playQueue_doubleClicked(const QModelIndex &index) {
    mAudioManager.playQueue()->setNextTrack(index.row());

    if (mAudioManager.paused())
	mAudioManager.togglePause();
    else
	mAudioManager.skipTrack();
}

