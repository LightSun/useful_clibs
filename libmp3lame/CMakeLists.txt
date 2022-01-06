CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

project(mp3lame LANGUAGES C CXX)

#-DSTDC_HEADERS
add_definitions("-DHAVE_CONFIG_H")

include_directories(lamemp3)
include_directories(lamemp3/mpglib)

aux_source_directory(lamemp3 SRC_LIST)
aux_source_directory(lamemp3/mpglib SRC_LIST2)

add_library(mp3lame
    ${SRC_LIST}
    ${SRC_LIST2}
#     h7_common.h
#     bitstream.c
#     encoder.c
#     fft.c
#     gain_analysis.c
#     id3tag.c
#     lame.c
#     mpglib_interface.c
#     newmdct.c
#     presets.c
#     psymodel.c
#     quantize.c
#     quantize_pvt.c
#     reservoir.c
#     set_get.c
#     tables.c
#     takehiro.c
#     util.c
#     vbrquantize.c
#     VbrTag.c
#     version.c

#     mpglib/common.c
#     mpglib/dct64_i386.c
#     mpglib/decode_i386.c
#     mpglib/interface.c
#     mpglib/layer1.c
#     mpglib/layer2.c
#     mpglib/layer3.c
#     mpglib/tabinit.c
     h7/lame_dec_api.h
     h7/lame_dec_mp3.h
     h7/lame_dec_mp3.c
)

add_executable(testlame h7/main.cpp)
target_link_libraries(testlame mp3lame)
