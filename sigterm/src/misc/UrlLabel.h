#ifndef _URLLABEL_H
#define _URLLABEL_H

#include <QLabel>

class UrlLabel : public QLabel {
	public:
		UrlLabel(QWidget* parent = 0);

		void setUrl(const QString& inUrl);

	protected:
		void mouseReleaseEvent (QMouseEvent * event);

	private:
		QString mUrl;

};

#endif

