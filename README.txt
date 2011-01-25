#########################################
OpenXcom v0.2
by SupSuper
#########################################

OpenXcom is an open-source reimplementation of the popular
UFO: Enemy Unknown (X-Com: UFO Defense in USA) videogame by
Microprose, licensed under the GPL and written in C++ / SDL.
See more info at the website: http://openxcom.ninex.info

1. For players
================

1.1. Windows users
-------------------

Extract the ZIP into a folder of your choice and copy the
contents of your X-Com folder into the DATA subfolder.

Run OpenXcom.exe to play.

1.2. Mac users
---------------

Extract the ZIP into a folder of your choice and copy the
contents of your X-Com folder into the application’s DATA
resource (right click on the application >
Show Package Contents > Contents > Resources > DATA).

Run OpenXcom.app to play.

1.3. Linux users
-----------------

The game requires the following libraries:
- SDL (libsdl1.2): http://www.libsdl.org
- SDL_mixer (libsdl-mixer1.2): http://www.libsdl.org/projects/SDL_mixer/
- SDL_gfx (libsdl-gfx1.2): http://www.ferzkopp.net/joomla/content/view/19/14/
- yaml-cpp: http://code.google.com/p/yaml-cpp/

Check your distribution's package manager or the library
website on how to install them.

NOTE: You need SDL_gfx 2.0.22 or later to properly view
the game. Earlier versions will experience graphical
glitches.

Extract the TAR into a folder of your choice and copy the
contents of your X-Com folder into the DATA subfolder.

Run openxcom to play.


2. For developers
===================

The game requires the following developer libraries:
- SDL: http://www.libsdl.org
- SDL_mixer: http://www.libsdl.org/projects/SDL_mixer/
- SDL_gfx: http://www.ferzkopp.net/joomla/content/view/19/14/
- yaml-cpp: http://code.google.com/p/yaml-cpp/

The source code includes a Visual C++ project, a Code::Blocks
project and a Linux makefile.

More detailed compiling instructions and pre-compiled
dependencies are available at:
http://openxcom.ninex.info/index.php/compiling/