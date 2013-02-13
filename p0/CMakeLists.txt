file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(p0 ${files})

target_link_libraries(p0 p0compile p0run p0rt p0i)
if (NOT MSVC)
	target_link_libraries(p0 ${Boost_LIBRARIES})
endif()
