file(GLOB files
	"*.hpp"
	"*.cpp")

add_library(p0i ${files})

install(TARGETS p0i ARCHIVE DESTINATION lib)
