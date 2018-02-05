add_executable(forward_class forward_class.cpp)

include_directories(
	${EXPLORE_INCLUDE}
	${Boost_INCLUDE_DIRS}
)

target_link_libraries(forward_class ${Boost_LIBRARIES})