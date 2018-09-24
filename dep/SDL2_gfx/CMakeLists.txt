set ( sdl2_gfx_src
	SDL2_framerate.c
	SDL2_gfxPrimitives.c
	SDL2_imageFilter.c
	SDL2_rotozoom.c
)

include_directories(
  ${SDL2_SOURCE_DIR}/include
)

add_library(SDL2Gfx STATIC ${sdl2_gfx_src} )

set_target_properties(SDL2Gfx
    PROPERTIES
      FOLDER
        "dep")