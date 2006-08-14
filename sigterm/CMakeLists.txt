PROJECT(sigterm)

INCLUDE(UsePkgConfig)

FIND_PACKAGE(Qt4)
FIND_PACKAGE(SDL)

PKGCONFIG(vorbisfile VORBISFILE_INCLUDE_DIR VORBISFILE_LINK_DIR VORBISFILE_LINK_FLAGS VORBISFILE_CFLAGS)

INCLUDE_DIRECTORIES(${VORBISFILE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${QT_INCLUDES})
INCLUDE_DIRECTORIES(src/audio/)

SET(SIGTERM_SRCS
src/audio/AudioFile.cc src/audio/AudioFile.h
src/audio/AudioFormat.cc src/audio/AudioFormat.h
src/audio/AudioManager.cc src/audio/AudioManager.h
src/audio/AudioConverter.cc src/audio/AudioConverter.h
src/audio/decoders/AudioFileOgg.cc src/audio/decoders/AudioFileOgg.h
src/main/main.cc
)

ADD_EXECUTABLE(sigterm ${SIGTERM_SRCS})

TARGET_LINK_LIBRARIES(sigterm ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${SDL_LIBRARY} ${VORBISFILE_LINK_FLAGS})

