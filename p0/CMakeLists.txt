
file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(p0 ${files})

target_link_libraries(p0 p0compile p0i ${Boost_LIBRARIES})
