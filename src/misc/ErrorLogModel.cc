#include "ErrorLogModel.h"
#include <QTreeView>
#include <QHeaderView>

ErrorLogModel::ErrorLogModel() {
}

void ErrorLogModel::postError(const QString &inErrorMessage) {
	beginInsertRows(QModelIndex(), 0, 0);
	mErrorLog.prepend(QPair<QDateTime, QString>(QDateTime::currentDateTime(), inErrorMessage));
	endInsertRows();
}

void ErrorLogModel::setupUi(QTreeView *inTree) {
	inTree->header()->resizeSection(ErrorLogModel::eDateTime, 150);
	inTree->header()->setStretchLastSection(true);
	inTree->header()->setSelectionMode(QAbstractItemView::SingleSelection);
	inTree->setDragEnabled(false);
	inTree->setAcceptDrops(false);
	inTree->setDropIndicatorShown(false);
}

QVariant ErrorLogModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (role == Qt::DisplayRole) {
		switch ((Columns)section) {
			case eDateTime:
				return "Time";
			case eMessage:
				return "Message";
			case eLastElement:
				break;
		}
	}

	return QVariant();
}

int ErrorLogModel::columnCount(const QModelIndex &parent) const {
	return eLastElement;
}

int ErrorLogModel::rowCount(const QModelIndex &parent) const {
	if (!parent.isValid())
		return mErrorLog.size();
	return 0;
}

QVariant ErrorLogModel::data(const QModelIndex &index, int role) const {
	if (index.isValid() == false)
		return QVariant();

	QPair<QDateTime, QString> pair = mErrorLog[index.row()];

	switch ((Columns)index.column()) {
		case eDateTime:
			return pair.first.toString();

		case eMessage:
			return pair.second;

		case eLastElement:
			break;
	}

	return QVariant();
}

bool ErrorLogModel::hasChildren(const QModelIndex &parent) const {
	if (parent.isValid() == false)
		return true;
	return false;
}

