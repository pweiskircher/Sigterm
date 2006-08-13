#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([sigterm], [0.0.1], [pat@icore.at])
AC_CONFIG_SRCDIR([src/audio/AudioFile.cc])
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

dnl PKG_CHECK_MODULES([SDL], [sdl >= 1.2.11])
AM_PATH_SDL

dnl PKG_CHECK_MODULES([OGG], [ogg >= 1.1.0])
dnl PKG_CHECK_MODULES([VORBIS], [vorbis >= 1.1.0])
PKG_CHECK_MODULES([VORBISFILE], [vorbisfile >= 1.1.0])

PKG_CHECK_MODULES([QT], [QtCore QtGui >= 4.1.0], HAVE_QT4="yes", HAVE_QT4="no")
if test "x$HAVE_QT4" = "xyes"; then
    dnl Check needed because in some cases the QtGui includedir
    dnl doesn't contain the subsystem dir.
    QT_INCLUDE_DIR=`$PKG_CONFIG --variable=includedir QtGui`
    EXTRA_QT_INCLUDE_DIR="$QT_INCLUDE_DIR/Qt"
    AC_CHECK_FILE([$QT_INCLUDE_DIR/QWidget],
		  AC_MSG_NOTICE([No extra QT_INCLUDE_DIR needed]),
		  AC_CHECK_FILE([$EXTRA_QT_INCLUDE_DIR/QWidget],
				QT_CFLAGS="$QT_CFLAGS -I$EXTRA_QT_INCLUDE_DIR",
				AC_MSG_WARN([QWidget not found])))
    AC_CHECK_PROG(MOC, [moc], [moc])
    AC_CHECK_PROG(MOC, [moc-qt4], [moc-qt4])
fi
if test "x$HAVE_QT4" = "xno"; then
    AC_MSG_NOTICE([Looking for QT_CFLAGS and QT_LIBS without pkg-config])
    case "$host_os" in
	darwin*)
	  AC_ARG_WITH([qt4dir],
		      [AS_HELP_STRING([--with-qt4dir=DIR],
				      [Qt4 installation directory used for OS-X.
				       For other systems use pkg-config.])],
				       [QT4DIR=$withval]
				       )
	  if test x"$QT4DIR" = x ; then
	      AC_MSG_ERROR([*** No path for Qt4 --with-qt4dir option given])
	  fi
	  AC_MSG_RESULT([QT4DIR... $QT4DIR])
	  AC_CHECK_PROG(MOC, [moc], [$QT4DIR/bin/moc], [], $QT4DIR/bin)
	  if test x"$MOC" = x; then
	      AC_MSG_ERROR([*** This is not the right Qt installation])
	  fi
	  QT_CFLAGS="-F$QT4DIR/lib -I$QT4DIR/lib/QtCore.framework/Headers"
	  QT_CFLAGS="$QT_CFLAGS -I$QT4DIR/lib/QtGui.framework/Headers"
	  QT_LIBS="-Xlinker -F$QT4DIR/lib -Xlinker -framework -Xlinker QtCore"
	  QT_LIBS="$QT_LIBS -Xlinker -framework -Xlinker QtGui"
	  ;;
	*)
	  AC_MSG_ERROR([*** Please check PKG_CONFIG_PATH or the version
			of your installed Qt4 installation.])
	  ;;
      esac
fi
AC_MSG_NOTICE([Set QT_CFLAGS... $QT_CFLAGS])
AC_SUBST(QT_CFLAGS)
AC_SUBST(QT_LIBS)

modules="audio main"
DEFAULT_INCLUDES=
SIGTERM_LIBS=
SIGTERM_DEPENDENCIES=
for module in $modules; do
    if ! test "$module" = "main"; then
	SIGTERM_LIBS="$SIGTERM_LIBS -L\$(top_srcdir)/src/${module} -l${module}"
	SIGTERM_DEPENDENCIES="$SIGTERM_DEPENDENCIES \$(top_srcdir)/src/${module}/lib${module}.la"
    fi
    DEFAULT_INCLUDES="$DEFAULT_INCLUDES -I\$(top_srcdir)/src/${module}"
done

SIGTERM_LIBS="$SIGTERM_LIBS $QT_LIBS $SDL_LIBS $VORBISFILE_LIBS"

AC_SUBST(SIGTERM_LIBS)
AC_SUBST(SIGTERM_DEPENDENCIES)

CPPFLAGS="$CPPFLAGS $DEFAULT_INCLUDES $QT_CFLAGS $SDL_CFLAGS $VORBISFILE_CFLAGS"

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_HEADER_STDC
AC_OUTPUT([Makefile
	   src/Makefile
	   src/audio/Makefile
	   src/audio/decoders/Makefile
	   src/main/Makefile
	   ])
