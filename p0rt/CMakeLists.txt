file(GLOB files
	"*.hpp"
	"*.cpp")

add_library(p0rt ${files})
target_link_libraries(p0rt p0run)

