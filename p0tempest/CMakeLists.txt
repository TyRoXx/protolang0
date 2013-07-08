file(GLOB files
	"*.hpp"
	"*.cpp")

add_library(p0tempest ${files})
target_link_libraries(p0tempest p0run ${P0_TEMPEST_LIBRARIES} ${Boost_LIBRARIES})

if(UNIX)
	target_link_libraries(p0tempest pthread)
endif()

install(TARGETS p0tempest ARCHIVE DESTINATION lib)
