#include "LastFMDialog.h"
#include "ErrorLogModel.h"

LastFMDialog::LastFMDialog(LastFMClient *inLastFMClient, QWidget *parent) {
	mClient = inLastFMClient;

	setupUi(this);

	mLog->setModel(&mErrorLog);
	mErrorLog.setupUi(mLog);
}

void LastFMDialog::postError(const QString &inErrorMessage) {
	mErrorLog.postError(inErrorMessage);
}
