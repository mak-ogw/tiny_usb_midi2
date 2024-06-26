cmake_minimum_required(VERSION 3.18)
project( lib-others LANGUAGES CXX )

# microshell
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/microshell/src)
add_library(microshell ${USH_SRC_FILES})
target_include_directories(microshell PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/microshell/src
    ${CMAKE_CURRENT_SOURCE_DIR}/microshell/src/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/microshell/src/config/arduino
)

# libmidi2(except cxx_std_11)
set(src_libmidi2
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/src/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/src/bytestreamToUMP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/src/umpToBytestream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/src/umpToMIDI1Protocol.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/src/umpMessageCreate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/src/midiCIMessageCreate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/src/midiCIProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/src/umpProcessor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/src/mcoded7.cpp
)
add_library(libmidi2 ${src_libmidi2})
target_include_directories(libmidi2 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/AM_MIDI2.0Lib/include
)

