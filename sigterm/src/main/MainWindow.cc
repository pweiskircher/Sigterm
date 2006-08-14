#include "MainWindow.h"
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    connect(&mAudioManager, SIGNAL(audioPaused(bool)), SLOT(audioPaused(bool)));
    mAudioManager.init();
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
}

void MainWindow::on_actionQuit_activated() {
    qApp->quit();
}

void MainWindow::on_playButton_clicked() {
    mAudioManager.togglePause();
}
