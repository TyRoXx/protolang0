cmake_minimum_required(VERSION 2.8)

project(protolang0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(UNIX OR MINGW)
	set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -pedantic")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0501")
endif()

if(NOT MSVC)
	set(Boost_USE_STATIC_LIBS OFF)
	find_package(Boost REQUIRED unit_test_framework program_options filesystem date_time system context)
	include_directories(${Boost_INCLUDE_DIRS})
endif()

include_directories(".")

if(UNIX)
	add_definitions("-DBOOST_TEST_DYN_LINK")
endif()

set(P0_TEMPEST_DIR "${CMAKE_SOURCE_DIR}/libtempest")
option(P0_WITH_TEMPEST "build the Tempest HTTP server for P0" OFF)
if(P0_WITH_TEMPEST)
	set(P0_TEMPEST_INSTALL_DIR ${CMAKE_BINARY_DIR}/libtempest)
	link_directories(${P0_TEMPEST_INSTALL_DIR}/lib)
	set(P0_TEMPEST_LIBRARIES tempest http)
	include(ExternalProject)
	ExternalProject_Add(libtempest
		SOURCE_DIR ${P0_TEMPEST_DIR}
		INSTALL_DIR ${P0_TEMPEST_INSTALL_DIR}
		CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${P0_TEMPEST_INSTALL_DIR}
		)
	add_definitions("-DPROTOLANG0_WITH_TEMPEST")
	include_directories(${P0_TEMPEST_DIR})
endif()

add_subdirectory(p0i)
add_subdirectory(p0optimize)
add_subdirectory(p0compile)
add_subdirectory(p0run)
add_subdirectory(p0c)
add_subdirectory(p0rt)
if(P0_WITH_TEMPEST)
	add_subdirectory(p0tempest)
endif()
add_subdirectory(p0)
add_subdirectory(test)
add_subdirectory(speed-test)

file(GLOB headers "p0common/*.hpp")
install(FILES ${headers} DESTINATION include/p0common)

file(GLOB headers "p0i/*.hpp")
install(FILES ${headers} DESTINATION include/p0i)

file(GLOB headers "p0optimize/*.hpp")
install(FILES ${headers} DESTINATION include/p0optimize)

file(GLOB headers "p0run/*.hpp")
install(FILES ${headers} DESTINATION include/p0run)

file(GLOB headers "p0rt/*.hpp")
install(FILES ${headers} DESTINATION include/p0rt)
