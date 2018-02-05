add_executable(binary_dump binary_dump.cpp)


include_directories(
	${EXPLORE_INCLUDE}
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(binary_dump ${Boost_LIBRARIES})