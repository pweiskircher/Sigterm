#include "Preferences.h"

Preferences::Preferences(QSettings *inSettings, QWidget *parent) : QDialog(parent) {
	setupUi(this);

	mSettings = inSettings;

	connect(mAutoPlay, SIGNAL(stateChanged(int)), SLOT(autoPlayStateChanged(int)));

	initUi();
}

Preferences::~Preferences() {
}

bool Preferences::autoPlayEnabled() {
	return mAutoPlay->checkState() == Qt::Checked ? true : false;
}

void Preferences::initUi() {
	bool autoPlay = mSettings->value("Main/StatePlayAutomatically", false).toBool();
	mAutoPlay->setChecked(autoPlay ? Qt::Checked : Qt::Unchecked);
}

void Preferences::autoPlayStateChanged(int state) {
	mSettings->setValue("Main/StatePlayAutomatically", state == Qt::Checked ? true : false);
}
