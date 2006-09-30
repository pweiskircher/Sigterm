#ifndef _PREFERENCES_H
#define _PREFERENCES_H

#include "ui_preferences.h"
#include <QSettings>

class Preferences : public QDialog, private Ui::Preferences {
	Q_OBJECT

	public:
		Preferences(QSettings *inSettings, QWidget *parent = 0);
		~Preferences();

		bool autoPlayEnabled();

	private slots:
		void autoPlayStateChanged(int state);

	private:
		void initUi();
		QSettings *mSettings;

};

#endif

