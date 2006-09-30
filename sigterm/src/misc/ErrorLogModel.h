#ifndef _ERROR_LOG_MODEL_H
#define _ERROR_LOG_MODEL_H

#include <QAbstractItemModel>
#include <QPair>
#include <QList>
#include <QString>
#include <QDateTime>

class QTreeView;

class ErrorLogModel : public QAbstractTableModel {
	public:
		ErrorLogModel();

		void postError(const QString &inErrorMessage);

		void setupUi(QTreeView *inTree);

		typedef enum {
			eDateTime,
			eMessage,
			eLastElement
		} Columns;

		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		int columnCount(const QModelIndex &parent) const;
		int rowCount(const QModelIndex &parent) const;
		QVariant data(const QModelIndex &index, int role) const;
		bool hasChildren(const QModelIndex &parent) const;

	private:
		QList< QPair<QDateTime, QString> > mErrorLog;
};

#endif
