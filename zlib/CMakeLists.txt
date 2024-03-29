cmake_minimum_required(VERSION 3.5)

project(zlib)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_definitions (-DNO_VIZ)

# adjust warnings
if (MSVC)
    add_definitions (-wd4131) # uses old-style declarator
else ()
    add_definitions (-DHAVE_UNISTD_H)
endif ()

set (ZLIB_LIBRARIES z_bundled PARENT_SCOPE)
add_library (z_bundled STATIC
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    inflate.c
    infback.c
    inftrees.c
    inffast.c
    trees.c
    uncompr.c
    zutil.c
)

set_target_properties (${ZLIB_LIBRARIES} PROPERTIES
    COMPILE_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS}"
)
