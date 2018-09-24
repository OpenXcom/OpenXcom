# Makefile.am for the SDL2_gfx library

lib_LTLIBRARIES = libSDL2_gfx.la

libSDL2_gfxincludedir =		\
	$(includedir)/SDL2
libSDL2_gfxinclude_HEADERS =	\
	SDL2_framerate.h	\
	SDL2_gfxPrimitives.h	\
	SDL2_imageFilter.h	\
	SDL2_rotozoom.h

libSDL2_gfx_la_SOURCES =	\
	SDL2_framerate.c	\
	SDL2_gfxPrimitives.c	\
	SDL2_imageFilter.c	\
	SDL2_rotozoom.c

EXTRA_DIST =			\
	autogen.sh

libSDL2_gfx_la_LDFLAGS = 	\
	-no-undefined		\
	-release $(LT_RELEASE)	\
	-version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

%.o : %.rc
	$(WINDRES) $< $@

# Rule to build tar-gzipped distribution package
$(PACKAGE)-$(VERSION).tar.gz: distcheck

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = SDL2_gfx.pc

# Additional cleanup rules
DISTCLEANFILES =		\
	*~			\
	SDL2_gfx.sdf		\
	SDL2_gfx.suo		\
	SDL2_gfx.vcxproj.user	\
	autom4te.cache/*	\
	Win32/Debug/*		\
	Win32/Release/*
