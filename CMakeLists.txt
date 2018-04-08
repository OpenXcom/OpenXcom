project ( OpenXcom )

cmake_minimum_required ( VERSION 3.1 )
set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules" )
include(GNUInstallDirs)

# For yaml-cpp
set (CMAKE_CXX_STANDARD 11)


option ( DEV_BUILD "Development Build. Disable this for release builds" ON )
option ( BUILD_PACKAGE "Prepares build for creation of a package with CPack" ON )
option ( ENABLE_WARNING "Always show warnings (even for release builds)" OFF )
option ( FATAL_WARNING "Treat warnings as errors" OFF )
option ( ENABLE_CLANG_ANALYSIS "When building with clang, enable the static analyzer" OFF )
option ( CHECK_CCACHE "Check if ccache is installed and use it" OFF )
set ( MSVC_WARNING_LEVEL 3 CACHE STRING "Visual Studio warning levels" )
option ( FORCE_INSTALL_DATA_TO_BIN "Force installation of data to binary directory" OFF )
set ( DATADIR "" CACHE STRING "Where to place datafiles" )
set ( OPENXCOM_VERSION_STRING "" CACHE STRING "Version string (after x.x)" )

if ( CHECK_CCACHE )
    # Configure CCache if available
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
       set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
       set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
    endif(CCACHE_FOUND)
endif ()

if ( WIN32 )
  set ( default_deps_dir "${CMAKE_SOURCE_DIR}/deps" )
endif ()

if ( APPLE )
  set ( MACOS_SDLMAIN_M_PATH "${CMAKE_SOURCE_DIR}" CACHE STRING "Path to SDLMain.m file" )
  option ( CREATE_BUNDLE "Create a Mac OS application bundle" ON )
  if ( NOT EXISTS "${MACOS_SDLMAIN_M_PATH}" )
    message ( FATAL_ERROR "On Mac OS, SDLMain.m is required. Please set the MACOS_SDLMAIN_M_PATH variable" )
  endif ()
endif ()

set ( DEPS_DIR "${default_deps_dir}" CACHE STRING "Dependencies directory" )

# Add check for library (SDL_gfx, yaml-cpp )
if ( IS_DIRECTORY ${DEPS_DIR} )
   include_directories ( ${DEPS_DIR}/include/SDL ${DEPS_DIR}/include/yaml-cpp ${DEPS_DIR}/include )
   if ( CMAKE_CL_64 )
     link_directories ( ${DEPS_DIR}/lib/x64 )
   else ( )
     link_directories ( ${DEPS_DIR}/lib/Win32 )
   endif()
   set( SDL_LIBRARY SDL )
   set ( SDLGFX_LIBRARY SDL_gfx )
   set ( SDLMIXER_LIBRARY SDL_mixer )
   set ( SDLIMAGE_LIBRARY SDL_image )
   set ( YAMLCPP_LIBRARY yaml-cpp )
   set ( YAMLCPP_LIBRARY_DEBUG yaml-cppd )
else ( )
  find_package ( SDL2 COMPONENTS mixer gfx image)
  find_package ( Yaml_cpp 0.5.0)
  set ( YAMLCPP_LIBRARY_DEBUG ${YAMLCPP_LIBRARY} )

  if ( NOT SDL_FOUND )
    message ( FATAL_ERROR "Can't find SDL which is required" )
  else ()
    include_directories ( ${SDL_INCLUDE_DIR} )
    message ( "found SDL ${SDL_MAJOR}.${SDL_MINOR}.${SDL_MICRO} (${SDL_LIBRARY_DIRS}:${SDL_INCLUDE_DIR})" )
  endif ()

  if ( NOT SDLMIXER_FOUND )
    message ( FATAL_ERROR "Can't find SDL_mixer which is required" )
  else ()
    include_directories ( ${SDLMIXER_INCLUDE_DIR} )
    message ( "found SDL_mixer ${SDL_MIXER_MAJOR}.${SDL_MIXER_MINOR}.${SDL_MIXER_MICRO} (${SDL_MIXER_LIBRARY_DIRS}:${SDLMIXER_INCLUDE_DIR})" )
  endif ()

  if ( NOT SDLGFX_FOUND )
    message ( FATAL_ERROR "Can't find SDL_gfx which is required" )
  else ()
    include_directories ( ${SDLGFX_INCLUDE_DIR} )
    message ( "found SDL_gfx ${SDL_GFX_MAJOR}.${SDL_GFX_MINOR}.${SDL_GFX_MICRO} (${SDL_GFX_LIBRARY_DIRS}:${SDLGFX_INCLUDE_DIR})" )
  endif ()

  if ( NOT SDLIMAGE_FOUND )
    message ( FATAL_ERROR "Can't find SDL_image which is required" )
  else ()
    include_directories ( ${SDLIMAGE_INCLUDE_DIR} )
    message ( "found SDL_image ${SDL_IMAGE_MAJOR}.${SDL_IMAGE_MINOR}.${SDL_IMAGE_MICRO} (${SDL_IMAGE_LIBRARY_DIRS}:${SDLIMAGE_INCLUDE_DIR})" )
  endif ()

  if ( NOT YAMLCPP_FOUND )
    message ( FATAL_ERROR "Can't find yaml-cpp which is required" )
  else ()
    include_directories ( ${YAMLCPP_INCLUDE_DIR} )
    message ( "found yaml-cpp(${YAMLCPP_LIBRARY_DIRS}:${YAMLCPP_INCLUDE_DIR})" )
  endif ( NOT YAMLCPP_FOUND )
endif()

# Find OpenGL
find_package ( OpenGL )
if ( NOT OPENGL_FOUND )
  message ( WARNING "Can't find OpenGL; how does that even happen?\n"
    "Continuing building without OpenGL support."
    )
  add_definitions(-D__NO_OPENGL)
else ()
    include_directories ( ${OPENGL_INCLUDE_DIR} )
endif ()

# Read version number
set ( file "${CMAKE_SOURCE_DIR}/src/version.h" )
file ( READ ${file} lines )
string ( REGEX MATCH "[.]*OPENXCOM_VERSION_SHORT \"([0-9]).([0-9])" version_line "${lines}" )
set ( CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_MATCH_1} )
set ( CPACK_PACKAGE_VERSION_MINOR ${CMAKE_MATCH_2} )
set ( CPACK_PACKAGE_VERSION_PATCH "" )

if ( "${OPENXCOM_VERSION_STRING}" STREQUAL "" )
  find_package ( Git )
  if ( GIT_FOUND )
    message("git found: ${GIT_EXECUTABLE}")
    execute_process ( COMMAND ${GIT_EXECUTABLE} describe --dirty
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE git_describe_out
      ERROR_VARIABLE git_describe_error
      RESULT_VARIABLE git_describe_result
      )
    string ( REGEX MATCH "([a-z|0-9|.]*)-([0-9]*)-g([a-z|0-9]*)([-|a-z]*)" git_commit "${git_describe_out}" )
    set ( git_tag ${CMAKE_MATCH_1} )
    set ( git_nb_commit ${CMAKE_MATCH_2} )
    set ( git_commit ${CMAKE_MATCH_3} )
    set ( git_dirty ${CMAKE_MATCH_4} )
    set ( OPENXCOM_VERSION_STRING ".${git_commit}${git_dirty}" )
  endif()
endif()

add_definitions( -DGIT_BUILD=1 )

configure_file("${CMAKE_SOURCE_DIR}/src/git_version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/git_version.h" )
include_directories ( "${CMAKE_CURRENT_BINARY_DIR}" )


if ( DEV_BUILD )
  # Append the commit to version number
  set ( CPACK_PACKAGE_VERSION_PATCH "${git_commit}${git_dirty}" )
  set ( CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}" )
else ()
  set ( CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_NSIS_PACKAGE_NAME}" )
  set ( CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}" )
endif ()

if ( BUILD_PACKAGE )
  if ( NOT DEV_BUILD )
    string ( LENGTH "${git_dirty}" is_dirty )
    if ( ${is_dirty} GREATER 0 )
      message ( FATAL_ERROR "Release package must be built from a clean tree" )
    endif ()
    if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
      message ( FATAL_ERROR "Release package can't be built from a debug build" )
    endif ()
  endif ( )

  if ( NOT CPACK_GENERATOR )
    set ( CPACK_GENERATOR "TXZ" )
  endif ()
  if ( NOT CPACK_SOURCE_GENERATOR )
    set ( CPACK_SOURCE_GENERATOR "TXZ" )
  endif ()

  set ( CPACK_PACKAGE_VENDOR "The OpenXcom project" )
  set ( CPACK_PACKAGE_DESCRIPTION_SUMMARY "Open-source clone of UFO: Enemy Unknown" )
  set ( CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/cmake/modules/Description.txt" )
  set ( CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md" )
  if ( NOT APPLE )
    set ( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt" )
  endif ()
  set ( CPACK_PACKAGE_CONTACT "The OpenXcom project (http://www.openxcom.org)" )

  include ( LinuxDEB )
  include ( LinuxRPM )
  include ( nsis )
  include ( apple )
  include ( CPack )
  message ( "version:${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"
)
endif()

if ( NOT WIN32 )
  install(FILES "${CMAKE_SOURCE_DIR}/res/linux/openxcom.desktop"
    DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications")
  install(FILES "${CMAKE_SOURCE_DIR}/res/linux/icons/openxcom_48x48.png"
    DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/hicolor/48x48/apps" RENAME openxcom.png)
  install(FILES "${CMAKE_SOURCE_DIR}/res/linux/icons/openxcom_128x128.png"
    DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/hicolor/128x128/apps" RENAME openxcom.png)
  install(FILES "${CMAKE_SOURCE_DIR}/res/linux/icons/openxcom.svg"
    DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/icons/hicolor/scalable/apps")
endif ()

add_subdirectory ( docs )
add_subdirectory ( src )
