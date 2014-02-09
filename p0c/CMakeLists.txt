file(GLOB files
	"*.hpp"
	"*.cpp")

add_executable(p0c ${files})

target_link_libraries(p0c p0compile p0optimize p0run p0i)
if (NOT MSVC)
	target_link_libraries(p0c ${Boost_LIBRARIES})
endif()

install(TARGETS p0c RUNTIME DESTINATION bin)
