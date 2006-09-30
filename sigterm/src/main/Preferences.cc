#include "Preferences.h"
#include "PWMd5.h"

Preferences::Preferences(QSettings *inSettings, QWidget *parent) : QDialog(parent) {
	setupUi(this);

	mSettings = inSettings;

	connect(mAutoPlay, SIGNAL(stateChanged(int)), SLOT(autoPlayStateChanged(int)));
	connect(mLastFMUsername, SIGNAL(editingFinished()), SLOT(lastFMUsernameEdited()));
	connect(mLastFMPassword, SIGNAL(editingFinished()), SLOT(lastFMPasswordEdited()));
}

Preferences::~Preferences() {
}

bool Preferences::autoPlayEnabled() {
	return mAutoPlay->checkState() == Qt::Checked ? true : false;
}

QString Preferences::lastFMUsername() {
	return mLastFMUsername->text();
}

QString Preferences::lastFMHashedPassword() {
	return PWMd5::md5sum(mLastFMPassword->text());
}

void Preferences::init() {
	bool autoPlay = mSettings->value("Main/StatePlayAutomatically", false).toBool();
	mAutoPlay->setChecked(autoPlay ? Qt::Checked : Qt::Unchecked);

	mLastFMUsername->setText(mSettings->value("LastFM/Username", "").toString());
	mLastFMPassword->setText(mSettings->value("LastFM/HashedPassword", "").toString());
	emit lastFMSettingsChanged(lastFMUsername(), lastFMHashedPassword());
}

void Preferences::autoPlayStateChanged(int state) {
	mSettings->setValue("Main/StatePlayAutomatically", state == Qt::Checked ? true : false);
}

void Preferences::lastFMUsernameEdited() {
	qDebug("username edided");
	mSettings->setValue("LastFM/Username", lastFMUsername());
	emit lastFMSettingsChanged(lastFMUsername(), lastFMHashedPassword());
}

void Preferences::lastFMPasswordEdited() {
	qDebug("password edided");
	mSettings->setValue("LastFM/HashedPassword", lastFMHashedPassword());
	emit lastFMSettingsChanged(lastFMUsername(), lastFMHashedPassword());
}

