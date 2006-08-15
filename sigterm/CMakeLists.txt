PROJECT(sigterm)

INCLUDE(UsePkgConfig)
INCLUDE(FindQt4)

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(SDL REQUIRED)

PKGCONFIG(vorbisfile VORBISFILE_INCLUDE_DIR VORBISFILE_LINK_DIR VORBISFILE_LINK_FLAGS VORBISFILE_CFLAGS)

INCLUDE_DIRECTORIES(${VORBISFILE_INCLUDE_DIR})
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


SET(MACOSX_BUNDLE_INFO_STRING	"Sigterm Music Player")
# SET(MACOSX_BUNDLE_ICON_FILE
SET(MACOSX_BUNDLE_GUI_IDENTIFIER "at.sigterm.sigterm")
# SET(MACOSX_BUNDLE_LONG_VERSION_STRING 
SET(MACOSX_BUNDLE_BUNDLE_NAME "sigterm")
# SET(MACOSX_BUNDLE_SHORT_VERSION_STRING
# SET(MACOSX_BUNDLE_BUNDLE_VERSION
# SET(MACOSX_BUNDLE_COPYRIGHT

ADD_EXECUTABLE(sigterm MACOSX_BUNDLE ${SIGTERM_SRCS} ${SIGTERM_UI_H} ${SIGTERM_MOC_SRCS})

TARGET_LINK_LIBRARIES(sigterm ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${SDL_LIBRARY} ${VORBISFILE_LINK_FLAGS})

