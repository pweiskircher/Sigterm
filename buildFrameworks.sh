#!/bin/sh

if [ ! -f MacOS/sigterm ]; then
	echo "You have to call $0 from the sigterm.app/Contents directory (file MacOS/sigterm not found)"
	exit 1;
fi

echo "Checking dependencies of MacOS/sigterm ..."
LIBS=`otool -L MacOS/sigterm | awk '{print $1}' | grep -v '/usr/lib' | grep -v '/System' | grep -v ':'`

echo "Removing Frameworks directory ..."
rm -rf Frameworks

echo "Creating Frameworks directory ..."
mkdir Frameworks

COPIED_LIBS=""
for LIB in $LIBS; do
	if [ `echo $LIB | grep .framework` ]; then
		FRAMEWORK=`echo $LIB | sed -e 's/\(.*.framework\).*$/\1/'`
		echo cp -R $FRAMEWORK Frameworks ...
		cp -R $FRAMEWORK Frameworks

		BASE=`basename $FRAMEWORK`

		echo rm -rf Frameworks/$BASE/Versions/Current/Headers
		echo rm -rf Frameworks/$BASE/Versions/Current/*_debug

		rm -rf Frameworks/$BASE/Versions/Current/Headers
		rm -rf Frameworks/$BASE/Versions/Current/*_debug

		BINARY=`echo $BASE | cut -d '.' -f 1`

		echo Changing dependencies for Frameworks/$BASE
		install_name_tool -id @executable_path/../Frameworks/$BASE/Versions/4.0/$BINARY Frameworks/$BASE/Versions/4.0/$BINARY
		install_name_tool -change $LIB @executable_path/../Frameworks/$BASE/Versions/4.0/$BINARY MacOS/sigterm

		COPIED_LIBS="$COPIED_LIBS Frameworks/$BASE/Versions/4.0/$BINARY"
	else
		echo cp $LIB Frameworks
		cp $LIB Frameworks
		BASE=`basename $LIB`

		echo Changing dependencies for Frameworks/$BASE
		install_name_tool -id @executable_path/../Frameworks/$BASE Frameworks/$BASE
		install_name_tool -change $LIB @executable_path/../Frameworks/$BASE MacOS/sigterm

		COPIED_LIBS="$COPIED_LIBS Frameworks/$BASE"
	fi
done

for LIB in $COPIED_LIBS; do
	DEPS=`otool -L $LIB | awk '{print $1}' | grep -v '/usr/lib' | grep -v '/System' | grep -v '@executable_path' | grep -v ':'`

	for DEPLIB in $DEPS; do
		if [ `echo $DEPLIB | grep .framework` ]; then
			FRAMEWORK=`echo $DEPLIB | sed -e 's/\(.*.framework\).*$/\1/'`
			BASE=`basename $FRAMEWORK`
			BINARY=`echo $BASE | cut -d '.' -f 1`

			echo "Changing depencendy for $LIB ..."
			install_name_tool -change $DEPLIB @executable_path/../Frameworks/${BASE}/Versions/4.0/$BINARY $LIB
		else
			BASE=`basename $DEPLIB`
			echo "Changing depencendy for $LIB ..."
			install_name_tool -change $DEPLIB @executable_path/../Frameworks/$BASE $LIB
		fi
	done
done

