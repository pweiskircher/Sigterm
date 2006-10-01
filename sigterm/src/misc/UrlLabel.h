#ifndef _URLLABEL_H
#define _URLLABEL_H

#include <QLabel>

class UrlLabel : public QLabel {
Q_OBJECT
	
	public:
		UrlLabel(QWidget* parent = 0);
		~UrlLabel();

		void setUrl(const QString& inUrl);

	protected:
		void mouseReleaseEvent (QMouseEvent * event);

	private:
		QString mUrl;

};

#endif

