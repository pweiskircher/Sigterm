#include "MainWindow.h"
#include "PlayList.h"
#include <QFileDialog>
#include <QDebug>
#include "AudioFile.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    connect(&mAudioManager, SIGNAL(audioPaused(bool)), SLOT(audioPaused(bool)));
    mAudioManager.init();

    playlist->setModel(mAudioManager.currentPlayList());
}

void MainWindow::audioPaused(bool inPause) {
    if (inPause) {
	playButton->setText("Play");
    } else {
	playButton->setText("Pause");
    }
}

void MainWindow::on_addButton_clicked() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Add Music Files", "/home", "(*.ogg)");
    for (int i=0; i<files.size(); i++) {
	mAudioManager.currentPlayList()->add(new AudioFile(files[i], &mAudioManager));
	playlist->reset();
    }
}

void MainWindow::on_actionQuit_activated() {
    mAudioManager.quit();
    qApp->quit();
}

void MainWindow::on_playButton_clicked() {
    mAudioManager.togglePause();
}

void MainWindow::on_playlist_doubleClicked(const QModelIndex &index) {
    mAudioManager.currentPlayList()->setNextTrack(index.row());

    if (mAudioManager.paused())
	mAudioManager.togglePause();
    else
	mAudioManager.skipTrack();
}

