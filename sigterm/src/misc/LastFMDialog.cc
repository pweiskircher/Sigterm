#include "LastFMDialog.h"
#include "ErrorLogModel.h"

LastFMDialog::LastFMDialog(LastFMClient *inLastFMClient, QWidget *parent) {
	mClient = inLastFMClient;

	setupUi(this);

	mLog->setModel(&mErrorLog);
	mErrorLog.setupUi(mLog);

	mStatusLabel->setText("<a href=\"http://amd.co.at\">homepage</a>");
}

void LastFMDialog::postError(const QString &inErrorMessage) {
	mErrorLog.postError(inErrorMessage);
}
