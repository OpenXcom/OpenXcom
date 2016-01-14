# Locate SDL_mixer library
# This module defines
# YAMLCPP_LIBRARY, the name of the library to link against
# YAMLCPP_FOUND, if false, do not try to link to SDL
# YAMLCPP_INCLUDE_DIR, where to find SDL/SDL.h
#
# Created by Guillaume Chevallereau. This was influenced by the FindSDL.cmake 
# module.

#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
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

FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(PC_YAMLCPP QUIET yaml-cpp)


FIND_PATH(YAMLCPP_INCLUDE_DIR yaml.h
  HINTS
  ${PC_YAMLCPP_INCLUDEDIR}
  ${PC_YAMLCPP_INCLUDE_DIRS}
  $ENV{YAMLCPPDIR}
  PATH_SUFFIXES include/yaml-cpp include yaml-cpp
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local/include/yaml-cpp
  /usr/include/yaml-cpp
  /usr/local/include
  /usr/include
  /opt/include/yaml-cpp
  /opt/include
)

FIND_LIBRARY(YAMLCPP_LIBRARY 
  NAMES yaml-cpp
  HINTS
  ${PC_YAMLCPP_LIBDIR}
  ${PC_YAMLCPP_LIBRARY_DIRS}
  $ENV{YAMLCPPDIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /usr/local
  /usr
  /sw
  /opt/local
  /opt/csw
  /opt
)

if ( APPLE )
  if ( NOT YAMLCPP_LIBRARY )
    FIND_LIBRARY(YAMLCPP_LIBRARY 
      NAMES YAML
      HINTS
      $ENV{YAMLCPPDIR}
      PATH_SUFFIXES lib64 lib
      PATHS
      ~/Library/Frameworks
      /Library/Frameworks
      /usr/local
      /usr
      /sw
      /opt/local
      /opt/csw
      /opt
      )
  endif ()
endif ()

SET(YAMLCPP_FOUND "NO")

IF(YAMLCPP_LIBRARY AND YAMLCPP_INCLUDE_DIR)
  SET(YAMLCPP_FOUND "YES")
  SET(YAMLCPP_INCLUDE_DIR "${YAMLCPP_INCLUDE_DIR};${YAMLCPP_INCLUDE_DIR}/..")
ENDIF(YAMLCPP_LIBRARY AND YAMLCPP_INCLUDE_DIR)

set ( YAMLCPP_LIBRARY_DIRS "" )
foreach( my_yamlcpp_lib ${YAMLCPP_LIBRARY} )
  get_filename_component(_yamlcpp_my_lib_path "${my_yamlcpp_lib}" PATH)
  list(APPEND YAMLCPP_LIBRARY_DIRS ${_yamlcpp_my_lib_path})
endforeach()
list(REMOVE_DUPLICATES YAMLCPP_LIBRARY_DIRS)
