cmake_minimum_required(VERSION 3.2)

project(samples)

IF(WIN32)
	set(Boost_USE_STATIC_LIBS        ON)	
ENDIF()	
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)


SET(CMAKE_CXX_FLAGS "-std=c++1y")

FIND_PACKAGE( Boost 1.60 REQUIRED system thread regex)

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

set(EXPLORE_INCLUDE "${PROJECT_SOURCE_DIR}/include")

add_subdirectory(examples)