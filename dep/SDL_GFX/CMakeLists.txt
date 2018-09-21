set ( sdl2_gfx_src
	SDL_framerate.c
	SDL_gfxPrimitives.c
	SDL_imageFilter.c
	SDL_rotozoom.c
)

include_directories(
  ${SDL2_SOURCE_DIR}/include
)

add_library(SDL2Gfx STATIC ${sdl2_gfx_src} )

set_target_properties(SDL2Gfx
    PROPERTIES
      FOLDER
        "dep")