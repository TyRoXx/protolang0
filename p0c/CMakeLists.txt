
file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(p0c ${files})

target_link_libraries(p0c p0i)
