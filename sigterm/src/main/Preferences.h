#ifndef _PREFERENCES_H
#define _PREFERENCES_H

#include "ui_preferences.h"

class Preferences : public QDialog, private Ui::Preferences {
	Q_OBJECT

	public:
		Preferences(QWidget *parent = 0);
		~Preferences();
};

#endif

