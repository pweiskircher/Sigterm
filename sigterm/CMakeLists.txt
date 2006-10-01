#### set up the bomb

PROJECT(sigterm)
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

INCLUDE(UsePkgConfig)
INCLUDE(FindQt4)

#### find generic libraries

FIND_PACKAGE(Qt4 REQUIRED)

INCLUDE_DIRECTORIES(${QT_INCLUDES})

#### find audio output libraries

FIND_PACKAGE(SDL REQUIRED)
IF(WIN32)
  # use _TEMP var, it should contain the path to SDL.dll only
  SET(SIGTERM_SDL_LIBRARY ${SDL_LIBRARY_TEMP})
ELSE(WIN32)
  # on unix, everything is sane
  SET(SIGTERM_SDL_LIBRARY ${SDL_LIBRARY})
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})

#### find audio input libraries

PKGCONFIG(vorbisfile VORBISFILE_INCLUDE_DIR VORBISFILE_LINK_DIR VORBISFILE_LINK_FLAGS VORBISFILE_CFLAGS)
IF(NOT VORBISFILE_INCLUDE_DIR)
	MESSAGE(FATAL_ERROR "Cant find required library: vorbisfile ${VORBISFILE_INCLUDE_DIR} ${VORBISFILE_CFLAGS}")
ENDIF(NOT VORBISFILE_INCLUDE_DIR)

FIND_LIBRARY(MAD_LIBRARY mad /usr/lib /usr/local/lib /opt/local/lib)
FIND_PATH(MAD_INCLUDE_DIR mad.h /usr/include /usr/local/include /opt/local/include)

FIND_LIBRARY(ID3TAG_LIBRARY id3tag /usr/lib /usr/local/lib /opt/local/lib)
FIND_PATH(ID3TAG_INCLUDE_DIR id3tag.h /usr/include /usr/local/include /opt/local/include)

FIND_LIBRARY(FLAC_LIBRARY FLAC /usr/lib /usr/local/lib /opt/local/lib)
FIND_PATH(FLAC_INCLUDE_DIR FLAC/file_decoder.h /usr/include /usr/local/include /opt/local/include)

FIND_LIBRARY(FAAD_LIBRARY faad /usr/lib /usr/local/lib /opt/local/lib)
FIND_PATH(FAAD_INCLUDE_DIR neaacdec.h /usr/include /usr/local/include /opt/local/include)

INCLUDE_DIRECTORIES(src/audio/ src/audio/aac_stuff/ src/library/ src/misc/ src/main/)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${VORBISFILE_INCLUDE_DIR} ${MAD_INCLUDE_DIR} ${FLAC_INCLUDE_DIR} ${FAAD_INCLUDE_DIR} ${ID3TAG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(src/3rdParty)

SET(SIGTERM_SRCS
src/audio/AudioDecoder.cc src/audio/AudioDecoder.h
src/audio/AudioFile.cc src/audio/AudioFile.h
src/audio/AudioFormat.cc src/audio/AudioFormat.h
src/audio/AudioManager.cc src/audio/AudioManager.h
src/audio/AudioConverter.cc src/audio/AudioConverter.h
src/audio/AudioStorage.cc src/audio/AudioStorage.h
src/audio/AudioProcessor.cc src/audio/AudioProcessor.h
src/audio/AudioLibrary.cc src/audio/AudioLibrary.h
src/audio/PlayQueue.cc src/audio/PlayQueue.h
src/audio/AudioBuffer.cc src/audio/AudioBuffer.h
src/audio/AudioMetaData.cc src/audio/AudioMetaData.h
src/audio/decoders/AudioDecoderFlac.cc src/audio/decoders/AudioDecoderFlac.h
src/audio/decoders/AudioDecoderMp3.cc src/audio/decoders/AudioDecoderMp3.h
src/audio/decoders/AudioDecoderMp4.cc src/audio/decoders/AudioDecoderMp4.h
src/audio/decoders/AudioDecoderOgg.cc src/audio/decoders/AudioDecoderOgg.h
src/3rdParty/md5.c src/3rdParty/md5.h
src/library/Library.cc src/library/Library.h
src/misc/LastFMClient.cc src/misc/LastFMClient.h
src/misc/LastFMDialog.cc src/misc/LastFMDialog.h
src/misc/PlaylistM3u.cc src/misc/PlaylistM3u.h
src/misc/PWMd5.cc src/misc/PWMd5.h
src/misc/ErrorLogModel.cc src/misc/ErrorLogModel.h
src/misc/UrlLabel.cc src/misc/UrlLabel.h
src/main/main.cc
src/main/MainWindow.cc src/main/MainWindow.h
src/main/Preferences.cc src/main/Preferences.h
src/main/PlayQueueView.cc src/main/PlayQueueView.h
src/main/AboutWindow.cc src/main/AboutWindow.h
)


SET(SIGTERM_UI
src/main/mainwindow.ui
src/main/preferences.ui
src/main/aboutwindow.ui
src/misc/lastfm.ui
)

SET(SIGTERM_MOC
src/main/MainWindow.h
src/main/Preferences.h
src/main/PlayQueueView.h
src/main/AboutWindow.h
src/audio/AudioProcessor.h
src/audio/AudioManager.h
src/audio/AudioFile.h
src/audio/PlayQueue.h
src/misc/LastFMClient.h
)

SET(SIGTERM_RC
src/main/res/sigterm.qrc
)

QT4_ADD_RESOURCES(SIGTERM_RCC_SRCS ${SIGTERM_RC})
QT4_WRAP_CPP(SIGTERM_MOC_SRCS ${SIGTERM_MOC})
QT4_WRAP_UI(SIGTERM_UI_H ${SIGTERM_UI})

SET(SIGTERM_LIBRARIES
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTSQL_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  ${SIGTERM_SDL_LIBRARY}
  ${FLAC_LIBRARY}
  ${FAAD_LIBRARY}
  ${MAD_LIBRARY}
  ${ID3TAG_LIBRARY}
)

SET(AAC_STUFF
src/audio/aac_stuff/aacinfo.c
src/audio/aac_stuff/aacinfo.h
src/audio/aac_stuff/drms.c
src/audio/aac_stuff/drms.h
src/audio/aac_stuff/drmstables.h
src/audio/aac_stuff/mp4atom.c
src/audio/aac_stuff/mp4ff.c
src/audio/aac_stuff/mp4ff.h
src/audio/aac_stuff/mp4ff_int_types.h
src/audio/aac_stuff/mp4ffint.h
src/audio/aac_stuff/mp4meta.c
src/audio/aac_stuff/mp4sample.c
src/audio/aac_stuff/mp4tagupdate.c
src/audio/aac_stuff/mp4util.c
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

ADD_DEFINITIONS(-DUSE_TAGGING)

ADD_EXECUTABLE(sigterm WIN32 MACOSX_BUNDLE ${SIGTERM_SRCS} ${SIGTERM_UI_H} ${SIGTERM_MOC_SRCS} ${AAC_STUFF} ${SIGTERM_RCC_SRCS})

# hack for broken pkgconfig on win32
IF(WIN32)
  TARGET_LINK_LIBRARIES(sigterm ${SIGTERM_LIBRARIES} -libpath:${VORBISFILE_LINK_DIR} ${VORBISFILE_LINK_FLAGS})
ELSE(WIN32)
  # sane unix stuff
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  TARGET_LINK_LIBRARIES(sigterm ${SIGTERM_LIBRARIES} ${VORBISFILE_LINK_FLAGS})
ENDIF(WIN32)


