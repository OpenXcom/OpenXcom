################
# OpenXcom 1.0 #
################

OpenXcom is an open-source clone of the popular
UFO: Enemy Unknown (X-Com: UFO Defense in USA) videogame by
Microprose, licensed under the GPL and written in C++ / SDL.

See more info at the website: http://openxcom.org
And the wiki: http://ufopaedia.org/index.php?title=OpenXcom

Uses modified code from SDL_gfx (LGPL) with permission from author.

1. Installation
================

OpenXcom requires a vanilla copy of the original X-Com resources.
If you have the Steam version, you can find the X-Com game
folder in "Steam\steamapps\common\xcom ufo defense\XCOM".
Do not use modded versions (eg. XcomUtil) as they may cause bugs
and crashes.

When installing manually, copy the X-Com subfolders (GEODATA,
GEOGRAPH, MAPS, ROUTES, SOUND, TERRAIN, UFOGRAPH, UFOINTRO,
UNITS) to OpenXcom's Data folder: <game directory>\data\

The resources can be in a different folder as the OpenXcom data.
You can also specify your own path by passing the command-line
argument "-data <data path>" when running OpenXcom.

1.1. Windows
-------------

OpenXcom will also check the following folders:

- C:\Documents and Settings\<user>\My Documents\OpenXcom\data (Windows 2000/XP)
- C:\Users\<user>\Documents\OpenXcom\data (Windows Vista/7)

It's recommended you copy the resources to the "data" subfolder.
The installer will automatically detect a Steam installation
and copy the resources as necessary.

1.2. Mac OS X
--------------

OpenXcom will also check the following folders:

- <application resources>\data
- ~/Library/Application Support/OpenXcom/data

It's recommended you copy the resources to the application's "data"
resource (right click the application > Show Package Contents >
Contents > Resources > data).

1.3. Linux
-----------

OpenXcom requires the following libraries:

- SDL (libsdl1.2):
http://www.libsdl.org
- SDL_mixer (libsdl-mixer1.2):
http://www.libsdl.org/projects/SDL_mixer/
- TiMidity++ (timidity), if you need MIDI playback:
http://timidity.sourceforge.net/
- SDL_gfx (libsdl-gfx1.2), version 2.0.22 or later:
http://www.ferzkopp.net/joomla/content/view/19/14/
- SDL_image (libsdl-image1.2)
http://www.libsdl.org/projects/SDL_image/
- yaml-cpp, version 0.5.1 or later:
http://code.google.com/p/yaml-cpp/

Check your distribution's package manager or the library
website on how to install them.

According to the XDG standard, OpenXcom will also check the
following folders:

- $XDG_DATA_HOME/openxcom/data
- $XDG_DATA_DIRS/openxcom/data

Or if those variables aren't available:

- ~/.local/share/openxcom/data
- /usr/share/openxcom/data
- /usr/local/share/openxcom/data

Choose whichever you prefer.


2. Configuration
=================

OpenXcom has a variety of game settings and extras that can be
customized, both in-game and out-game. These options are global
and affect any old or new savegame.

For more details please check the wiki:
http://ufopaedia.org/index.php?title=Options_(OpenXcom)

2.1. User Folder
-----------------

OpenXcom creates a User folder with all the user screenshots,
savegames and options in one of the following paths:

- <game directory>\user\
- C:\Documents and Settings\<user>\My Documents\OpenXcom (Windows 2000/XP)
- C:\Users\<user>\Documents\OpenXcom (Windows Vista/7)
- ~/Library/Application Support/OpenXcom (Mac OS X)
- $XDG_DATA_HOME/openxcom (Linux)
- $XDG_CONFIG_HOME/openxcom (Linux)

You can also specify your own path by passing the command-line
argument "-user <user path>" when running OpenXcom.


3. Development
===============

OpenXcom requires the following developer libraries:

- SDL (libsdl1.2):
http://www.libsdl.org
- SDL_mixer (libsdl-mixer1.2):
http://www.libsdl.org/projects/SDL_mixer/
- SDL_gfx (libsdl-gfx1.2), version 2.0.22 or later:
http://www.ferzkopp.net/joomla/content/view/19/14/
- SDL_image (libsdl-image1.2):
http://www.libsdl.org/projects/SDL_image/
- yaml-cpp, version 0.5.1 or later:
http://code.google.com/p/yaml-cpp/

The source code includes files for the following tools:

- Microsoft Visual C++ 2010 or newer.
- Xcode.
- Makefile.
- CMake.
- Autotools.

It's also been tested on a variety of other tools on
Windows/Mac/Linux. More detailed compiling instructions
and pre-compiled dependencies are available at:
http://ufopaedia.org/index.php?title=Compiling_(OpenXcom)
