#!/bin/sh

LIBS=`otool -L MacOS/sigterm | awk '{print $1}' | grep -v '/usr/lib' | grep -v '/System'`
echo $LIBS
rm -rf Frameworks
mkdir Frameworks
for LIB in $LIBS; do
	echo $LIB
	if [ `echo $LIB | grep .framework` ]; then
		FRAMEWORK=`echo $LIB | sed -e 's/\(.*.framework\).*$/\1/'`
		cp -R $FRAMEWORK Frameworks
		BASE=`basename $FRAMEWORK`
		rm -rf Frameworks/$BASE/Versions/Current/Headers
		rm -rf Frameworks/$BASE/Versions/Current/*_debug

		BINARY=`echo $BASE | cut -d '.' -f 1`
		install_name_tool -id @executable_path/../Frameworks/$BASE/Versions/4.0/$BINARY Frameworks/$BASE/Versions/4.0/$BINARY
		install_name_tool -change $LIB @executable_path/../Frameworks/$BASE/Versions/4.0/$BINARY MacOS/sigterm
	else
		cp $LIB Frameworks
		BASE=`basename $LIB`
		install_name_tool -id @executable_path/../Frameworks/$BASE Frameworks/$BASE
		install_name_tool -change $LIB @executable_path/../Frameworks/$BASE MacOS/sigterm
	fi
done

install_name_tool -change /usr/local/Trolltech/Qt-4.1.1/lib/QtCore.framework/Versions/4.0/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore Frameworks/QtGui.framework/Versions/4.0/QtGui
install_name_tool -change /usr/local/Trolltech/Qt-4.1.1/lib/QtCore.framework/Versions/4.0/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore Frameworks/QtSql.framework/Versions/4.0/QtSql
