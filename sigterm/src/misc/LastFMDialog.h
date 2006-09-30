#ifndef _LASTFM_DIALOG_H
#define _LASTFM_DIALOG_H

#include "ui_lastfm.h"
#include "ErrorLogModel.h"

class LastFMClient;

class LastFMDialog : public QDialog, private Ui::LastFMDialog {
	public:
		LastFMDialog(LastFMClient *inLastFMClient, QWidget *parent = 0);

		void postError(const QString &inErrorMessage);

	private:
		LastFMClient *mClient;
		ErrorLogModel mErrorLog;
};

#endif
