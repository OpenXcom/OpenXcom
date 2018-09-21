
set(lib_SRCS
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

include_directories(
  ${CMAKE_SOURCE_DIR}/dep/zlib
)

add_library(libpng STATIC ${lib_SRCS} )

set_target_properties(libpng
    PROPERTIES
      FOLDER
        "dep")