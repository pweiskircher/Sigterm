#include "LastFMDialog.h"
#include "LastFMClient.h"
#include "ErrorLogModel.h"

#include <QSortFilterProxyModel>
#include <QHeaderView>

LastFMDialog::LastFMDialog(LastFMClient *inLastFMClient, QWidget *parent) {
	mClient = inLastFMClient;

	setupUi(this);

	mLog->setModel(&mErrorLog);
	mErrorLog.setupUi(mLog);

	QSortFilterProxyModel *sortProxy = new QSortFilterProxyModel(this);
	sortProxy->setSourceModel(inLastFMClient->queue());
	sortProxy->sort(LastFMQueue::eDatePlayed, Qt::DescendingOrder);
	mSubmitQueue->setModel(sortProxy);
	inLastFMClient->queue()->setupUi(mSubmitQueue);
}

void LastFMDialog::postError(const QString &inErrorMessage) {
	mErrorLog.postError(inErrorMessage);
}
