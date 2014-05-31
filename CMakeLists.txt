cmake_minimum_required(VERSION 2.6)

project(LFWATCH)

# Bump up warning levels appropriately for each compiler
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11 -ggdb")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
	endif()
endif()

if (NOT DEFINED NO_SDL)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LFWATCH_SOURCE_DIR}/cmake")
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIR})
else()
	add_definitions(-DNO_SDL)
endif()

include_directories(include)
add_subdirectory(src)
add_subdirectory(demo)

