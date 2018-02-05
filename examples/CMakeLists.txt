project(examples)

add_executable(fileinput fileinput.cpp)
add_executable(sample sample.cpp)

include_directories(
	${EXPLORE_INCLUDE}
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(fileinput ${Boost_LIBRARIES})
target_link_libraries(sample ${Boost_LIBRARIES})

add_subdirectory(forward)
add_subdirectory(parser)