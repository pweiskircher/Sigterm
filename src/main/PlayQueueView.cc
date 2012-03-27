#include "PlayQueueView.h"
#include "PlayQueue.h"
#include <QDebug>
#include <QHeaderView>

PlayQueueView::PlayQueueView(QWidget* parent) : QTreeView(parent) {
}

PlayQueueView::~PlayQueueView() {
}

void PlayQueueView::setup(PlayQueue *inQueue) {
	setModel(inQueue);

	header()->resizeSection(PlayQueue::eIsPlaying, 20);
	header()->resizeSection(PlayQueue::eTrackNumber, 25);
	header()->resizeSection(PlayQueue::eArtist, 100);
	header()->resizeSection(PlayQueue::eAlbum, 100);
	header()->resizeSection(PlayQueue::eTitle, 150);
	header()->setResizeMode(PlayQueue::eTitle, QHeaderView::Stretch);
	header()->resizeSection(PlayQueue::eTotalTime, 20);

	header()->setStretchLastSection(false);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setDragEnabled(true);
	setAcceptDrops(true);
	setDropIndicatorShown(true);
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
