#include "PWMd5.h"
#include "md5.h"

QString PWMd5::md5sum(const QString &inString) {
	MD5_CTX ctx;

	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char *)inString.toUtf8().constData(), inString.toUtf8().size());

	unsigned char result[16];
	MD5Final(result, &ctx);

	QString m;
	for (int i = 0; i < 16; i++) {
		m += QString::number(result[i], 16).toUpper();
	}

	return m;
}
