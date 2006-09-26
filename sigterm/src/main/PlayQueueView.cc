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
				emit removeSelectedTracksKeyPressed();
				return;
		}
	}
	
	QTreeView::keyPressEvent(event);
}
