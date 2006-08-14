PROJECT(sigterm)

INCLUDE(UsePkgConfig)

FIND_PACKAGE(Qt4)
FIND_PACKAGE(SDL)

PKGCONFIG(vorbisfile VORBISFILE_INCLUDE_DIR VORBISFILE_LINK_DIR VORBISFILE_LINK_FLAGS VORBISFILE_CFLAGS)

INCLUDE_DIRECTORIES(${VORBISFILE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(src/audio/)

SET(SIGTERM_SRCS
src/audio/AudioFile.cc src/audio/AudioFile.h
src/audio/AudioFormat.cc src/audio/AudioFormat.h
src/audio/AudioManager.cc src/audio/AudioManager.h
src/audio/AudioConverter.cc src/audio/AudioConverter.h
src/audio/decoders/AudioFileOgg.cc src/audio/decoders/AudioFileOgg.h
src/main/main.cc
src/main/MainWindow.cc src/main/MainWindow.h
)

SET(SIGTERM_UI
src/main/mainwindow.ui
)

SET(SIGTERM_MOC
src/main/MainWindow.h
)

QT4_WRAP_CPP(SIGTERM_MOC_SRCS ${SIGTERM_MOC})
QT4_WRAP_UI(SIGTERM_UI_H ${SIGTERM_UI})

ADD_EXECUTABLE(sigterm MACOSX_BUNDLE ${SIGTERM_SRCS} ${SIGTERM_UI_H} ${SIGTERM_MOC_SRCS})

TARGET_LINK_LIBRARIES(sigterm ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${SDL_LIBRARY} ${VORBISFILE_LINK_FLAGS})

