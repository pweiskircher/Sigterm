#include "MainWindow.h"
#include "PlayList.h"
#include <QFileDialog>
#include <QDebug>
#include <QHeaderView>
#include "AudioFile.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    connect(&mAudioManager, SIGNAL(audioPaused(bool)), SLOT(audioPaused(bool)));
    connect(qApp, SIGNAL(lastWindowClosed()), SLOT(on_actionQuit_activated()));
    mAudioManager.init();

    playList->setModel(mAudioManager.currentPlayList());
    playList->header()->resizeSection(0, 20);
    playList->header()->resizeSection(1, 500);
    playList->header()->setResizeMode(1, QHeaderView::Stretch);
    playList->header()->resizeSection(2, 50);
    playList->header()->setStretchLastSection(false);
}

void MainWindow::audioPaused(bool inPause) {
    if (inPause) {
	playButton->setText("Play");
    } else {
	playButton->setText("Pause");
    }
}

void MainWindow::on_addButton_clicked() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Add Music Files", "/home", "(*.flac);;(*.ogg)");
    for (int i=0; i<files.size(); i++) {
	mAudioManager.currentPlayList()->add(new AudioFile(files[i], &mAudioManager));
	playList->reset();
    }
}

void MainWindow::on_actionQuit_activated() {
    mAudioManager.quit();
    qApp->quit();
}

void MainWindow::on_playButton_clicked() {
    mAudioManager.togglePause();
}

void MainWindow::on_playList_doubleClicked(const QModelIndex &index) {
    mAudioManager.currentPlayList()->setNextTrack(index.row());

    if (mAudioManager.paused())
	mAudioManager.togglePause();
    else
	mAudioManager.skipTrack();
}

