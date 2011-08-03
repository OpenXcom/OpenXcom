project ( OpenXcom )

cmake_minimum_required ( VERSION 2.8 )
set ( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}" )
message ( "${CMAKE_MODULE_PATH}" )

# Add check for library (SDL_gfx, yaml-cpp )

include ( FindSDL )
include ( FindSDL_mixer )
include ( FindSDL_gfx )
include ( FindYaml_cpp )

if ( NOT SDL_FOUND )
  message ( FATAL_ERROR "SDL is needed" )
else ()
  include_directories ( ${SDL_INCLUDE_DIR} ) 
endif ()

if ( NOT SDLMIXER_FOUND )
  message ( FATAL_ERROR "SDL_mixer is needed" )
else ()
  include_directories ( ${SDLMIXER_INCLUDE_DIR} ) 
endif ()

if ( NOT SDLGFX_FOUND )
  message ( FATAL_ERROR "SDL_gfx is needed" )
else ()
  include_directories ( ${SDLGFX_INCLUDE_DIR} ) 
endif ()

if ( NOT YAMLCPP_FOUND )
  message ( FATAL_ERROR "yaml-cpp is needed" )
else ()
  include_directories ( ${YAMLCPP_INCLUDE_DIR} ) 
endif ( NOT YAMLCPP_FOUND )

add_subdirectory ( src )
