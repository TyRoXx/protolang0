cmake_minimum_required(VERSION 2.8)

project(protolang0)

if(UNIX) #gcc or clang
	list(APPEND CMAKE_CXX_FLAGS "-std=c++0x -g")
endif(UNIX)

include_directories(".")

add_subdirectory(p0i)
add_subdirectory(p0c)
add_subdirectory(p0)
