cmake_minimum_required(VERSION 2.6.2)

project( myproject )

add_subdirectory(googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})


add_subdirectory( src )
