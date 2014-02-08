file(GLOB files
	"*.hpp"
	"*.cpp")

add_library(p0optimize ${files})

install(TARGETS p0optimize ARCHIVE DESTINATION lib)
