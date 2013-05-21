file(GLOB files
	"*.hpp"
	"*.cpp")

add_library(p0compile ${files})

install(TARGETS p0compile ARCHIVE DESTINATION lib)
