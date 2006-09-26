#include "PlayQueueView.h"
#include <QDebug>

PlayQueueView::PlayQueueView(QWidget* parent) : QTreeView(parent) {
}

PlayQueueView::~PlayQueueView() {
}

void PlayQueueView::keyPressEvent(QKeyEvent* event) {

	if (model()) {
		
		switch(event->key()) {
			case Qt::Key_Backspace:
			case Qt::Key_Delete:
				event->accept();
				const QItemSelection selection = selectionModel()->selection();
				QList<QItemSelectionRange>::const_iterator it = selection.begin();
				for (; it != selection.end(); ++it) {
					QModelIndex parent = (*it).parent();
					if ((*it).left() != 0)
						continue;
					if ((*it).right() != (model()->columnCount(parent) - 1))
						continue;
					int count = (*it).bottom() - (*it).top() + 1;
					model()->removeRows((*it).top(), count, parent);
				}
				return;
		}
	}
	
	QTreeView::keyPressEvent(event);
}
