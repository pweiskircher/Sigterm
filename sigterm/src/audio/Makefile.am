SUBDIRS = decoders

noinst_LTLIBRARIES = libaudio.la

libaudio_la_LDFLAGS = -avoid-version
libaudio_la_DEPENDENCIES = decoders/libdecoders.la
libaudio_la_LIBADD = -Ldecoders -ldecoders
libaudio_la_SOURCES = AudioFile.cc \
			AudioFormat.cc \
			AudioManager.cc
