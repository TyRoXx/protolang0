cmake_minimum_required(VERSION 2.8)

project(protolang0)

if (UNIX OR MINGW)
	set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -pedantic")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

find_package(Boost REQUIRED unit_test_framework program_options filesystem date_time system)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(".")

if (UNIX)
	add_definitions("-DBOOST_TEST_DYN_LINK")
endif()

add_subdirectory(p0i)
add_subdirectory(p0compile)
add_subdirectory(p0run)
add_subdirectory(p0c)
add_subdirectory(p0rt)
add_subdirectory(p0)
add_subdirectory(test)
add_subdirectory(speed-test)

file(GLOB headers "p0common/*.hpp")
install(FILES ${headers} DESTINATION include/p0common)

file(GLOB headers "p0i/*.hpp")
install(FILES ${headers} DESTINATION include/p0i)

file(GLOB headers "p0run/*.hpp")
install(FILES ${headers} DESTINATION include/p0run)

file(GLOB headers "p0rt/*.hpp")
install(FILES ${headers} DESTINATION include/p0rt)
