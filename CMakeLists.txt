cmake_minimum_required(VERSION 2.8)

project(protolang0)

if(CMAKE_COMPILER_IS_GNUCXX )
	list(APPEND CMAKE_CXX_FLAGS "-std=c++0x -g")
endif()

include_directories(".")

add_subdirectory(p0i)
add_subdirectory(p0compile)
add_subdirectory(p0c)
add_subdirectory(p0)
add_subdirectory(test)
