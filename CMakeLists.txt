project ( OpenXcom )

cmake_minimum_required ( VERSION 2.8 )
set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )

option ( DEV_BUILD "Developement Build. Turn off for release" ON )
option ( BUILD_PACKAGE "Add need rules to build packages" ON )
option ( ENABLE_WARNING "Always show warnings(even for release build" OFF )
option ( FATAL_WARNING "Treats warnings as errors" OFF )
set ( MSVC_WARNING_LEVEL 3 CACHE STRING "Visual Studio warning levels" )

if ( WIN32 )
  set ( default_deps_dir "${CMAKE_SOURCE_DIR}/deps" )
endif ()

set ( DEPS_DIR "${default_deps_dir}" CACHE STRING "Deps directory" )

# Add check for library (SDL_gfx, yaml-cpp )
if ( IS_DIRECTORY ${DEPS_DIR} )
   include_directories ( ${DEPS_DIR}/include/SDL ${DEPS_DIR}/include/yaml-cpp ${DEPS_DIR}/include )
   link_directories ( ${DEPS_DIR}/lib )
   set( SDL_LIBRARY SDL )
   set ( SDLGFX_LIBRARY SDL_gfx )
   set ( SDLMIXER_LIBRARY SDL_mixer )
   set ( YAMLCPP_LIBRARY yaml-cpp )
else ( )
  find_package ( SDL2 COMPONENTS mixer gfx )
  find_package ( Yaml_cpp )

  if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "SDL is needed" )
  else ()
    include_directories ( ${SDL_INCLUDE_DIR} )
    message ( "found SDL ${SDL_MAJOR}.${SDL_MINOR}.${SDL_MICRO} (${SDL_LIBRARY_DIRS}:${SDL_INCLUDE_DIR})" )
  endif ()

  if ( NOT SDLMIXER_FOUND )
    message ( FATAL_ERROR "SDL_mixer is needed" )
  else ()
    include_directories ( ${SDLMIXER_INCLUDE_DIR} )
    message ( "found SDL_mixer ${SDL_MIXER_MAJOR}.${SDL_MIXER_MINOR}.${SDL_MIXER_MICRO} (${SDL_MIXER_LIBRARY_DIRS}:${SDLMIXER_INCLUDE_DIR})" )
  endif ()

  if ( NOT SDLGFX_FOUND )
    message ( FATAL_ERROR "SDL_gfx is needed" )
  else ()
    include_directories ( ${SDLGFX_INCLUDE_DIR} )
    message ( "found SDL_gfx ${SDL_GFX_MAJOR}.${SDL_GFX_MINOR}.${SDL_GFX_MICRO} (${SDL_GFX_LIBRARY_DIRS}:${SDLGFX_INCLUDE_DIR})" )
  endif ()

  if ( NOT YAMLCPP_FOUND )
    message ( FATAL_ERROR "yaml-cpp is needed" )
  else ()
    include_directories ( ${YAMLCPP_INCLUDE_DIR} )
    message ( "found yaml-cpp(${YAMLCPP_LIBRARY_DIRS}:${YAMLCPP_INCLUDE_DIR})" )
  endif ( NOT YAMLCPP_FOUND )
endif()

# Read version number
set ( file "${CMAKE_SOURCE_DIR}/src/Engine/Options.cpp" )
file ( READ ${file} lines )
string ( REGEX MATCH "[.]*_version = \"([0-9]).([0-9])" version_line "${lines}" )
set ( CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_MATCH_1} )
set ( CPACK_PACKAGE_VERSION_MINOR ${CMAKE_MATCH_2} )
set ( CPACK_PACKAGE_VERSION_PATCH "" )
find_package ( Git )
if ( GIT_FOUND )
  message("git found: ${GIT_EXECUTABLE}")
  execute_process ( COMMAND ${GIT_EXECUTABLE} describe --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE git_describe_out
    ERROR_VARIABLE git_describe_error
    RESULT_VARIABLE git_describe_result
    )
  string ( REGEX MATCH "([a-z|0-9|.]*)-([0-9]*)-([a-z|0-9]*)([-|a-z]*)" git_commit "${git_describe_out}" )
  set ( git_tag ${CMAKE_MATCH_1} )
  set ( git_nb_commit ${CMAKE_MATCH_2} )
  set ( git_commit ${CMAKE_MATCH_3} )
  set ( git_dirty ${CMAKE_MATCH_4} )
endif()

if ( DEV_BUILD )
  # Append the commit to version number
  set ( CPACK_PACKAGE_VERSION_PATCH "${git_commit}${git_dirty}" )
endif ()

if ( BUILD_PACKAGE )
  if ( NOT DEV_BUILD )
    string ( LENGTH "${git_dirty}" is_dirty )
    if ( ${is_dirty} GREATER 0 )
      message ( FATAL_ERROR "Released package should be built from a clean tree" )
    endif ()
    if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
      message ( FATAL_ERROR "Released package should not be built from a debug build" )
    endif ()
  endif ( )
  set ( CPACK_PACKAGE_VENDOR "The OpenXcom project" )
  set ( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.txt" )
  set ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING" )
  set ( CPACK_PACKAGE_CONTACT "The OpenXcom project(http://www.openxcom.org)" )
  set ( CPACK_DEBIAN_PACKAGE_MAINTAINER "The OpenXcom project" )
  include ( CPack )
  message ( "version:${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"
)
endif()

add_subdirectory ( docs )
add_subdirectory ( src )
