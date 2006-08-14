#include "MainWindow.h"
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    mAudioManager.init();
}

void MainWindow::on_addButton_clicked() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Add Music Files", "/home", "(*.ogg)");
}

void MainWindow::on_actionQuit_activated() {
    qApp->quit();
}
