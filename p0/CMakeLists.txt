file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(p0 ${files})

target_link_libraries(p0 p0compile p0run p0rt p0i)
if(NOT MSVC)
	target_link_libraries(p0 ${Boost_LIBRARIES})
endif()

if(P0_WITH_TEMPEST)
	target_link_libraries(p0 p0tempest)
endif()

install(TARGETS p0 RUNTIME DESTINATION bin)
