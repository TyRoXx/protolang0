cmake_minimum_required(VERSION 2.8)

project(protolang0)

if (CMAKE_COMPILER_IS_GNUCXX)
	list(APPEND CMAKE_CXX_FLAGS "-std=c++0x -g -Wall -Wconversion -Wextra -pedantic")
endif()

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

find_package(Boost REQUIRED unit_test_framework program_options)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(".")

if (UNIX)
	add_definitions("-DBOOST_TEST_DYN_LINK")
endif()

add_subdirectory(p0i)
add_subdirectory(p0compile)
add_subdirectory(p0run)
add_subdirectory(p0c)
add_subdirectory(p0)
add_subdirectory(test)
