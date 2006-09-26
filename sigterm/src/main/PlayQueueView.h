#ifndef _PLAYQUEUEVIEW_H
#define _PLAYQUEUEVIEW_H

#include <QObject>
#include <QTreeView>
#include <QKeyEvent>

class PlayQueueView : public QTreeView {
	Q_OBJECT
	
	public:
		PlayQueueView(QWidget* parent = 0);
		~PlayQueueView();

	protected:
		void keyPressEvent(QKeyEvent* event);

};	

#endif //!_PLAYQUEUEVIEW_H
