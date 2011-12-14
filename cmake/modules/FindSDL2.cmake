# Locate SDL library
# This module defines
# SDL_LIBRARY, the name of the library to link against
# SDL_FOUND, if false, do not try to link to SDL
# SDL_INCLUDE_DIR, where to find SDL.h
#
# This module responds to the the flag:
# SDL_BUILDING_LIBRARY
# If this is defined, then no SDL_main will be linked in because 
# only applications need main().
# Otherwise, it is assumed you are building an application and this
# module will attempt to locate and set the the proper link flags
# as part of the returned SDL_LIBRARY variable.
#
# Don't forget to include SDLmain.h and SDLmain.m your project for the 
# OS X framework based version. (Other versions link to -lSDLmain which
# this module will try to find on your behalf.) Also for OS X, this 
# module will automatically add the -framework Cocoa on your behalf.
#
#
# Additional Note: If you see an empty SDL_LIBRARY_TEMP in your configuration
# and no SDL_LIBRARY, it means CMake did not find your SDL library 
# (SDL.dll, libsdl.so, SDL.framework, etc). 
# Set SDL_LIBRARY_TEMP to point to your SDL library, and configure again. 
# Similarly, if you see an empty SDLMAIN_LIBRARY, you should set this value
# as appropriate. These values are used to generate the final SDL_LIBRARY
# variable, but when these values are unset, SDL_LIBRARY does not get created.
#
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
# l.e.galup  9-20-02
#
# Modified by Eric Wing. 
# Added code to assist with automated building by using environmental variables
# and providing a more controlled/consistent search behavior.
# Added new modifications to recognize OS X frameworks and 
# additional Unix paths (FreeBSD, etc). 
# Also corrected the header search path to follow "proper" SDL guidelines.
# Added a search for SDLmain which is needed by some platforms.
# Added a search for threads which is needed by some platforms.
# Added needed compile switches for MinGW.
#
# On OSX, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of 
# SDL_LIBRARY to override this selection or set the CMake environment
# CMAKE_INCLUDE_PATH to modify the search paths.
#
# Note that the header path has changed from SDL/SDL.h to just SDL.h
# This needed to change because "proper" SDL convention
# is #include "SDL.h", not <SDL/SDL.h>. This is done for portability
# reasons because not all systems place things in SDL/ (see FreeBSD).
#
#
# Modified by Guillaume Chevallereau
# Added code to search for SDL sub library (e.g : mixer, gfx )
# This avoid the multiplication of FindSDL_*.cmake modules.
# To use, just add a COMPONENTS component1 component2 to find_package.
# example: find_package ( SDL2 COMPONENTS mixer gfx )
#          Will look for SDL, SDL_mixer and SDL_gfx.
# For each components, SDL${UPPERCOMPONENT}_FOUND will be set if the component is found. the module will also set SDL${UPPERCOMPONENT}_LIBRARY for the library and SDL${UPPERCOMPONENT}_INCLUDE_DIR) for include dir of the library. UPPERCOMPONENT is the name of the component in upper-case(mixer => MIXER)
# Each component is first search in the SDL${UPPERCOMPONENT}DIR environment variable, then in the found SDL dir. The search is then performed in the standard directory of the platform.

#=============================================================================
# Copyright 2003-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

FIND_PATH(SDL_INCLUDE_DIR SDL.h
  HINTS
  $ENV{SDLDIR}
  PATH_SUFFIXES include/SDL include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/SDL12
  /usr/local/include/SDL11 # FreeBSD ports
  /usr/include/SDL12
  /usr/include/SDL11
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)
#MESSAGE("SDL_INCLUDE_DIR is ${SDL_INCLUDE_DIR}")

# SDL-1.1 is the name used by FreeBSD ports...
# don't confuse it for the version number.
FIND_LIBRARY(SDL_LIBRARY_TEMP 
  NAMES SDL SDL-1.1
  HINTS
  $ENV{SDLDIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  /sw
  /opt/local
  /opt/csw
  /opt
)

#MESSAGE("SDL_LIBRARY_TEMP is ${SDL_LIBRARY_TEMP}")

IF(NOT SDL_BUILDING_LIBRARY)
  IF(NOT ${SDL_INCLUDE_DIR} MATCHES ".framework")
    # Non-OS X framework versions expect you to also dynamically link to 
    # SDLmain. This is mainly for Windows and OS X. Other (Unix) platforms 
    # seem to provide SDLmain for compatibility even though they don't
    # necessarily need it.
    FIND_LIBRARY(SDLMAIN_LIBRARY 
      NAMES SDLmain SDLmain-1.1
      HINTS
      $ENV{SDLDIR}
      PATH_SUFFIXES lib64 lib
      PATHS
      /sw
      /opt/local
      /opt/csw
      /opt
    )
  ENDIF(NOT ${SDL_INCLUDE_DIR} MATCHES ".framework")
ENDIF(NOT SDL_BUILDING_LIBRARY)

# SDL may require threads on your system.
# The Apple build may not need an explicit flag because one of the 
# frameworks may already provide it. 
# But for non-OSX systems, I will use the CMake Threads package.
IF(NOT APPLE)
  FIND_PACKAGE(Threads)
ENDIF(NOT APPLE)

# MinGW needs an additional library, mwindows
# It's total link flags should look like -lmingw32 -lSDLmain -lSDL -lmwindows
# (Actually on second look, I think it only needs one of the m* libraries.)
IF(MINGW)
  SET(MINGW32_LIBRARY mingw32 CACHE STRING "mwindows for MinGW")
ENDIF(MINGW)

SET(SDL_FOUND "NO")
IF(SDL_LIBRARY_TEMP)
  # For SDLmain
  IF(NOT SDL_BUILDING_LIBRARY)
    IF(SDLMAIN_LIBRARY)
      SET(SDL_LIBRARY_TEMP ${SDLMAIN_LIBRARY} ${SDL_LIBRARY_TEMP})
    ENDIF(SDLMAIN_LIBRARY)
  ENDIF(NOT SDL_BUILDING_LIBRARY)

  # For OS X, SDL uses Cocoa as a backend so it must link to Cocoa.
  # CMake doesn't display the -framework Cocoa string in the UI even 
  # though it actually is there if I modify a pre-used variable.
  # I think it has something to do with the CACHE STRING.
  # So I use a temporary variable until the end so I can set the 
  # "real" variable in one-shot.
  IF(APPLE)
    SET(SDL_LIBRARY_TEMP ${SDL_LIBRARY_TEMP} "-framework Cocoa")
  ENDIF(APPLE)
    
  # For threads, as mentioned Apple doesn't need this.
  # In fact, there seems to be a problem if I used the Threads package
  # and try using this line, so I'm just skipping it entirely for OS X.
  IF(NOT APPLE)
    SET(SDL_LIBRARY_TEMP ${SDL_LIBRARY_TEMP} ${CMAKE_THREAD_LIBS_INIT})
  ENDIF(NOT APPLE)

  # For MinGW library
  IF(MINGW)
    SET(SDL_LIBRARY_TEMP ${MINGW32_LIBRARY} ${SDL_LIBRARY_TEMP})
  ENDIF(MINGW)

  # Set the final string here so the GUI reflects the final state.
  SET(SDL_LIBRARY ${SDL_LIBRARY_TEMP} CACHE STRING "Where the SDL Library can be found")
  # Set the temp variable to INTERNAL so it is not seen in the CMake GUI
  SET(SDL_LIBRARY_TEMP "${SDL_LIBRARY_TEMP}" CACHE INTERNAL "")

  SET(SDL_FOUND "YES")
ENDIF(SDL_LIBRARY_TEMP)

macro ( find_sdl_version _header_file _COMPONENT_NAME _major _minor _micro )
  file ( READ ${_header_file} lines )

  set ( define_base "SDL_${_COMPONENT_NAME}" )
  if ( "${_COMPONENT_NAME}" STREQUAL "" )
    set ( define_base "SDL" )
  endif ()

  string ( REGEX MATCH "#define ${define_base}_MAJOR[A-Z|_]*[ |	]([0-9])" sdl_comp_version "${lines}" )
  set ( ${_major} ${CMAKE_MATCH_1} )
  string ( REGEX MATCH "#define ${define_base}_MINOR[A-Z|_]*[ |	]([0-9])" sdl_comp_version1 "${lines}" )
  set ( ${_minor} ${CMAKE_MATCH_1} )
  string ( REGEX MATCH "#define ${define_base}_PATCHLEVEL[ |	]*([0-9]*)" sdl_comp_version2 "${lines}" )
  set ( ${_micro} ${CMAKE_MATCH_1} )
  if ( "${${_micro}}" STREQUAL "" )
    string ( REGEX MATCH "#define ${define_base}_MICRO[A-Z|_]*[ |	]*([0-9]*)" sdl_comp_version2 "${lines}" )
    set ( ${_micro} ${CMAKE_MATCH_1} )
  endif ()
endmacro ()

macro ( FindSDL_component _component )
  string ( TOUPPER ${_component} UPPERCOMPONENT )
  set ( SDL${UPPERCOMPONENT}_FOUND "NO" )
  set ( SDL_header_name SDL_${_component}.h )
  set ( SDL_COMPONENT_NAME ${UPPERCOMPONENT} )

  #Special case for SDL_gfx. This seems to be the only library where the main header is not SDL_${lib}.h.
  if ( ${UPPERCOMPONENT} STREQUAL "GFX" )
    set ( SDL_header_name SDL_gfxPrimitives.h )
    set ( SDL_COMPONENT_NAME GFXPRIMITIVES )
  endif ()

  find_path ( SDL${UPPERCOMPONENT}_INCLUDE_DIR ${SDL_header_name}
    HINTS
    $ENV{SDL${UPPERCOMPONENT}DIR}
    $ENV{SDLDIR}
    PATH_SUFFIXES include/SDL include
    PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include/SDL12
    /usr/local/include/SDL11 # FreeBSD ports
    /usr/include/SDL12
    /usr/include/SDL11
    /sw # Fink
    /opt/local # DarwinPorts
    /opt/csw # Blastwave
    /opt
    )
  find_library ( SDL${UPPERCOMPONENT}_LIBRARY
    NAMES SDL_${_component}
    HINTS
    $ENV{SDL${UPPERCOMPONENT}DIR}
    $ENV{SDLDIR}
    PATH_SUFFIXES lib64 lib
    PATHS
    /sw
    /opt/local
    /opt/csw
    /opt
    )

  if ( SDL${UPPERCOMPONENT}_LIBRARY AND SDL${UPPERCOMPONENT}_INCLUDE_DIR )
    set ( SDL${UPPERCOMPONENT}_FOUND "YES" )
    set ( SDL${UPPERCOMPONENT}_MAINHEADER "${SDL${UPPERCOMPONENT}_INCLUDE_DIR}/${SDL_header_name}" )

    find_sdl_version ( "${SDL${UPPERCOMPONENT}_MAINHEADER}"
      "${SDL_COMPONENT_NAME}"
      SDL_${UPPERCOMPONENT}_MAJOR
      SDL_${UPPERCOMPONENT}_MINOR
      SDL_${UPPERCOMPONENT}_MICRO )

    set ( SDL_${UPPERCOMPONENT}_LIBRARY_DIRS "" )
    foreach( my_sdl_lib ${SDL${UPPERCOMPONENT}_LIBRARY} )
      get_filename_component(_sdl_my_lib_path "${my_sdl_lib}" PATH)
      list(APPEND SDL_${UPPERCOMPONENT}_LIBRARY_DIRS ${_sdl_my_lib_path})
    endforeach()
    list(REMOVE_DUPLICATES SDL_${UPPERCOMPONENT}_LIBRARY_DIRS)
  endif ()
endmacro ()

set ( SDL_LIBRARY_DIRS "" )
foreach( my_sdl_lib ${SDL_LIBRARY} )
  get_filename_component(_sdl_my_lib_path "${my_sdl_lib}" PATH)
  list(APPEND SDL_LIBRARY_DIRS ${_sdl_my_lib_path})
endforeach()
list(REMOVE_DUPLICATES SDL_LIBRARY_DIRS)

foreach ( component ${SDL2_FIND_COMPONENTS} )
  FindSDL_component ( ${component} )
endforeach()

find_sdl_version ( "${SDL_INCLUDE_DIR}/SDL_version.h"
  ""
  SDL_MAJOR
  SDL_MINOR
  SDL_MICRO )
