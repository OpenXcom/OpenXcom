set ( sdl2_image_src
    "IMG.c"
    "IMG_bmp.c"
    "IMG_gif.c"
    "IMG_jpg.c"
    "IMG_lbm.c"
    "IMG_pcx.c"
    "IMG_png.c"
    "IMG_pnm.c"
    "IMG_svg.c"
    "IMG_tga.c"
    "IMG_tif.c"
    "IMG_webp.c"
    "IMG_WIC.c"
    "IMG_xcf.c"
    "IMG_xpm.c"
    "IMG_xv.c"
    "IMG_xxx.c"
)

#SDL_IMAGE_USE_WIC_BACKEND
ADD_DEFINITIONS(-DLOAD_JPG=1)
ADD_DEFINITIONS(-DLOAD_BMP=1)
ADD_DEFINITIONS(-DLOAD_GIF=1)
ADD_DEFINITIONS(-DLOAD_LBM=1)
ADD_DEFINITIONS(-DLOAD_PCX=1)
ADD_DEFINITIONS(-DLOAD_PNG=1)
ADD_DEFINITIONS(-DLOAD_PNM=1)
ADD_DEFINITIONS(-DLOAD_SVG=1)
ADD_DEFINITIONS(-DLOAD_TGA=1)
ADD_DEFINITIONS(-DLOAD_TIF=1)
#ADD_DEFINITIONS(-DLOAD_WEBP=1)
ADD_DEFINITIONS(-DLOAD_XCF=1)
ADD_DEFINITIONS(-DLOAD_XPM=1)
ADD_DEFINITIONS(-DLOAD_XV=1)
ADD_DEFINITIONS(-DLOAD_XXX=1)

include_directories(
  ${SDL2_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/dep/jpeglib
  ${CMAKE_SOURCE_DIR}/dep/libpng
  ${CMAKE_SOURCE_DIR}/dep/libtiff/libtiff
  ${CMAKE_BINARY_DIR}/dep/libtiff/libtiff
)

add_library(SDL2Image STATIC ${sdl2_image_src} )

set_target_properties(SDL2Image
    PROPERTIES
      FOLDER
        "dep")