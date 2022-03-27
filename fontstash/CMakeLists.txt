cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
#2022-3-27
project(fontstash VERSION 1.0 LANGUAGES C)

include_directories(src)
include_directories(D:/study/cpp/study_font/glfw/include)
include_directories(D:/study/github/mine/useful_clibs/FreeType/include)

add_definitions(-DFONTSTASH_IMPLEMENTATION -DFONS_USE_FREETYPE)
SET(LIB_GLFW D:/study/cpp/study_font/build-glfw-Desktop_Qt_5_12_9_MinGW_64_bit-Debug/src/libglfw3.a)
SET(LIB_FREETYPE D:/study/github/mine/useful_clibs/build-FreeType-Desktop_Qt_5_12_9_MinGW_64_bit-Debug/libfreetype.dll.a)

add_executable(
    fontstash_demo
    src/fontstash.h
    src/gl3corefontstash.h
    src/glfontstash.h
   # src/stb_truetype.h
    src/fontstash.c
    example/example.c
    )
add_executable(
    fontstash_error
    src/fontstash.h
    src/gl3corefontstash.h
    src/glfontstash.h
   # src/stb_truetype.h
    src/fontstash.c
    example/error.c
    )

target_link_libraries(fontstash_demo ${LIB_GLFW} ${LIB_FREETYPE} Opengl32 glu32)
target_link_libraries(fontstash_error ${LIB_GLFW} ${LIB_FREETYPE} Opengl32 glu32)
