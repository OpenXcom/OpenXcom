# Locate SDL_mixer library
# This module defines
# YAMLCPP_LIBRARY, the name of the library to link against
# YAMLCPP_FOUND, if false, do not try to link to SDL
# YAMLCPP_INCLUDE_DIR, where to find SDL/SDL.h
#
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
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

FIND_PATH(YAMLCPP_INCLUDE_DIR yaml.h
  HINTS
  $ENV{YAMLCPPDIR}
  PATH_SUFFIXES include/yaml-cpp include
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

SET(YAMLCPP_FOUND "NO")

IF(YAMLCPP_LIBRARY AND YAMLCPP_INCLUDE_DIR)
  SET(YAMLCPP_FOUND "YES")
  SET(YAMLCPP_INCLUDE_DIR "${YAMLCPP_INCLUDE_DIR};${YAMLCPP_INCLUDE_DIR}/..")
ENDIF(YAMLCPP_LIBRARY AND YAMLCPP_INCLUDE_DIR)

