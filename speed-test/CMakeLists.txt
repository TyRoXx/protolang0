file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(speed-test ${files})

target_link_libraries(speed-test p0compile p0run p0rt p0i)
if (NOT MSVC)
	target_link_libraries(speed-test ${Boost_LIBRARIES})
endif()
