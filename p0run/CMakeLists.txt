file(GLOB files
	"*.hpp"
	"*.cpp")

add_library(p0run ${files})
target_link_libraries(p0run p0i)

install(TARGETS p0run ARCHIVE DESTINATION lib)
