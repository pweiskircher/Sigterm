#include <QProcess>
#include <QMessageBox>
#include "UrlLabel.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

UrlLabel::UrlLabel(QWidget* parent) : QLabel(parent) {
	raise();
}

void UrlLabel::setUrl(const QString& inUrl) {
	mUrl = inUrl;
	setText("<a href=\""+inUrl+"\">"+inUrl+"</a>");
}

void UrlLabel::mouseReleaseEvent (QMouseEvent * event) {
	bool success = false;
#ifdef Q_WS_WIN

	QString action("open");
	HINSTANCE status = ::ShellExecuteW(NULL, (LPCWSTR)action.utf16(), (LPCWSTR) mUrl.utf16(), NULL, NULL, SW_SHOW);
	
	// if (status>32)
	success = true;
#else

	QString program;
#ifdef Q_WS_MAC
	program = "open";
#else
	program = "gnome-open";
#endif
	
	QStringList args;
	args << mUrl;
	
	if (QProcess::startDetached(program, args))
		success = true;
#endif

	if (!success) {
		QMessageBox::critical(this, "URL", "Could not launch browser");
	}
}

