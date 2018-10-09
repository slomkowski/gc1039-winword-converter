cmake_minimum_required(VERSION 3.2)

project(word-converter)

set(CMAKE_SYSTEM_NAME Windows)

SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER i686-w64-mingw32-windres)
set(CMAKE_RANLIB i686-w64-mingw32-ranlib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)


set(SOURCE_FILES main.cpp convapi.h converr.h)

add_executable(word-converter ${SOURCE_FILES})
