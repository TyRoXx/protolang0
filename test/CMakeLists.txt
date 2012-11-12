
file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(test ${files})

target_link_libraries(test p0compile)
