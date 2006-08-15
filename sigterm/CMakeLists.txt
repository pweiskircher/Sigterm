PROJECT(sigterm)

IF(WIN32)
  MESSAGE(">> sigterm: Building for Win32")
  # use _TEMP var, it should contain the path to SDL.dll only
  SET(SIGTERM_SDL_LIBRARY ${SDL_LIBRARY_TEMP})
ELSE(WIN32)
  # on unix, everything is sane
  SET(SIGTERM_SDL_LIBRARY ${SDL_LIBRARY})
ENDIF(WIN32)

INCLUDE(UsePkgConfig)
INCLUDE(FindQt4)

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(SDL REQUIRED)

PKGCONFIG(vorbisfile VORBISFILE_INCLUDE_DIR VORBISFILE_LINK_DIR VORBISFILE_LINK_FLAGS VORBISFILE_CFLAGS)

FIND_LIBRARY(FLAC_LIBRARY FLAC++ /usr/lib /usr/local/lib /opt/local/lib)
FIND_PATH(FLAC_INCLUDE_DIR FLAC++/decoder.h /usr/include /usr/local/include /opt/local/include)

INCLUDE_DIRECTORIES(${VORBISFILE_INCLUDE_DIR} ${FLAC_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(src/audio/)

SET(SIGTERM_SRCS
src/audio/AudioDecoder.cc src/audio/AudioDecoder.h
src/audio/AudioFile.cc src/audio/AudioFile.h
src/audio/AudioFormat.cc src/audio/AudioFormat.h
src/audio/AudioManager.cc src/audio/AudioManager.h
src/audio/AudioConverter.cc src/audio/AudioConverter.h
src/audio/AudioStorage.cc src/audio/AudioStorage.h
src/audio/AudioProcessor.cc src/audio/AudioProcessor.h
src/audio/PlayList.cc src/audio/PlayList.h
src/audio/AudioBuffer.cc src/audio/AudioBuffer.h
src/audio/decoders/AudioDecoderOgg.cc src/audio/decoders/AudioDecoderOgg.h
src/main/main.cc
src/main/MainWindow.cc src/main/MainWindow.h
)

SET(SIGTERM_UI
src/main/mainwindow.ui
)

SET(SIGTERM_MOC
src/main/MainWindow.h
src/audio/AudioProcessor.h
src/audio/AudioManager.h
)

QT4_WRAP_CPP(SIGTERM_MOC_SRCS ${SIGTERM_MOC})
QT4_WRAP_UI(SIGTERM_UI_H ${SIGTERM_UI})

SET(SIGTERM_LIBRARIES
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${SIGTERM_SDL_LIBRARY}
  ${FLAC_LIBRARY}
)

SET(MACOSX_BUNDLE_INFO_STRING	"Sigterm Music Player")
# SET(MACOSX_BUNDLE_ICON_FILE
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "at.sigterm.sigterm")
# SET(MACOSX_BUNDLE_LONG_VERSION_STRING 
SET(MACOSX_BUNDLE_BUNDLE_NAME "sigterm")
# SET(MACOSX_BUNDLE_SHORT_VERSION_STRING
# SET(MACOSX_BUNDLE_BUNDLE_VERSION
# SET(MACOSX_BUNDLE_COPYRIGHT

# need qtmain on win32
IF(WIN32)
  SET(SIGTERM_SRCS ${SIGTERM_SRCS} src/main/qtmain_win.cpp)
ENDIF(WIN32)

ADD_EXECUTABLE(sigterm WIN32 MACOSX_BUNDLE ${SIGTERM_SRCS} ${SIGTERM_UI_H} ${SIGTERM_MOC_SRCS})

# hack for broken pkgconfig on win32
IF(WIN32)
  TARGET_LINK_LIBRARIES(sigterm ${SIGTERM_LIBRARIES} -libpath:${VORBISFILE_LINK_DIR} ${VORBISFILE_LINK_FLAGS})
ELSE(WIN32)
  # sane unix stuff
  TARGET_LINK_LIBRARIES(sigterm ${SIGTERM_LIBRARIES} ${VORBISFILE_LINK_FLAGS})
ENDIF(WIN32)
