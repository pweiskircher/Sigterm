#ifndef _PWMD5_H
#define _PWMD5_H

#include <QString>

class PWMd5 {
	public:
		static QString md5sum(const QString &inString);
};

#endif
