cmake_minimum_required(VERSION 3.5)

project(libpng)

#print msg on building
message(STATUS "libpng ar: ${CMAKE_AR}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(PNG_LIBRARIES png_bundled PARENT_SCOPE)
add_library(png_bundled STATIC
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
set_target_properties(png_bundled PROPERTIES
    COMPILE_FLAGS "${CMAKE_STATIC_LIBRARY_C_FLAGS} -DPNG_PREFIX=pa_"
)
