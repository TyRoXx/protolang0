
file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(p0 ${files})

target_link_libraries(p0 p0i)
