/*!

\mainpage SDL2_gfx - Graphics primitives and surface functions for SDL2

\section contact_sec Contact and License

Email aschiffler at ferzkopp dot net to contact the author 
or better check author's homepage at http://www.ferzkopp.net 
for the most up-to-date contact information.

This library is licenced under the zlib License, see the file LICENSE for details. 


\section intro_sec Introduction

The SDL2_gfx library provides the basic drawing functions such as lines,
circles or polygons provided by SDL_gfx on SDL2 against renderers of SDL2.

The current components of the SDL2_gfx library are:
- Graphic Primitives (SDL2_gfxPrimitives.h, SDL2_gfxPrimitives.c)
- Surface Rotozoomer (SDL2_rotozoom.h, SDL2_rotozoom.c)
- Framerate control (SDL2_framerate.h, SDL2_framerate.c)
- MMX image filters (SDL2_imageFilter.h, SDL2_imageFilter.c)
- Build-in 8x8 Font (SDL2_gfxPrimitives_font.h)

Note that SDL2_gfx is compatible with SDL version 2.0 (not SDL 1.2).

\section install_sec Installation

\subsection unix Unix/Linux

Use the standard autoconf/automake sequence to compile and install the library. 
\verbatim
        ./autogen.sh    # (optional, recommended)
        ./configure
        make
        make install
\endverbatim

\\subsubsection nommx Linker Configuration

The default location for the installation is /usr/local/lib and /usr/local/include. 
This libary path may need to be added to the file the linker configuration file:
\verbatim
        vi /etc/ld.so.conf
        ldconfig
\endverbatim

\\subsubsection nommx Non-MMX Platforms

To build without MMX code enabled (i.e. ARM, PPC, AMD64 architectures):
\verbatim
        ./configure --disable-mmx
        make
        make install
\endverbatim

\subsection visualstudio Windows (VS2015)

Open the SDL2_gfx.sln solution file, right click on the solution and choose 'Rebuild'.

The SDL2-2.0.5 folder must be placed in a directory alongside SDL2_gfx (or sdl2gfx-code) and build in the same configuration, i.e. Debug or Release, beforehand so the referenced SDL2.lib file can be found.

\subsection platformosx Mac OSX 

The usual autotools build chain should be used. MacPorts or fink may be required.

Xcode is supported via templates. See Xcode.zip - this template only supports SDL2_gfx 
and not the tests. For this template, the Deployment Target (the lowest version to run on) 
is set to 10.11 and expects the SDL2.framework preinstalled in the default location: /Library/Frameworks.

\section test_sec Test Programs

Change to the ./test directory and run
\verbatim
        ./autogen.sh
        ./configure
        make
\endverbatim
to create several test programs for the libraries functions. This requires
the SDL2_gfx library to be previously compiled and installed.

See the source in the test/*.c files for some sample code and implementation hints.

\section documentation_sec Documentation

Please refer to the Doxygen-generated API documentation found in the
Docs/html folder as well as the test programs in the test folder.

\section changelog_sec Change Log

\verbinclude ChangeLog

*/
