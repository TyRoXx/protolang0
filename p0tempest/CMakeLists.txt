file(GLOB files
	"*.hpp"
	"*.cpp")

add_library(p0tempest ${files})
target_link_libraries(p0tempest p0run tempest)

install(TARGETS p0tempest ARCHIVE DESTINATION lib)
