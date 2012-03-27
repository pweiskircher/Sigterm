#ifndef _PREFERENCES_H
#define _PREFERENCES_H

#include "ui_preferences.h"
#include <QSettings>

class Preferences : public QDialog, private Ui::Preferences {
	Q_OBJECT

	public:
		Preferences(QSettings *inSettings, QWidget *parent = 0);
		~Preferences();

		void init();

		bool autoPlayEnabled();
		QString lastFMUsername();
		QString lastFMHashedPassword();

	signals:
		void lastFMSettingsChanged(const QString &inUsername, const QString &inPassword);

	private slots:
		void autoPlayStateChanged(int state);

		void lastFMUsernameEdited();
		void lastFMPasswordEdited();

	private:
		QSettings *mSettings;

};

#endif

