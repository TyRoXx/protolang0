file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(test ${files})

target_link_libraries(test p0compile p0run p0i)
if (NOT MSVC)
	target_link_libraries(test ${Boost_LIBRARIES})
endif()
