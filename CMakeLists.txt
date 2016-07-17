# Minimum cmake version
cmake_minimum_required(VERSION 2.8)

project(MemoryManager)

# Library includes
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/source/
	${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# Library sources
file(GLOB_RECURSE LIBRARY_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

# Flags for g++/clang
if (UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif (UNIX)

# Add library
add_library(memorymanager ${LIBRARY_SRC_FILES})

# Test sources
file(GLOB_RECURSE TEST_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/test/*.h
)