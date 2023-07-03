```cmake
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

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# Test sources
file(GLOB_RECURSE TEST_SRC_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/test/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/source/memory_manager_test.cpp
)

# Now simply link against gtest or gtest_main as needed.
add_executable(tests ${TEST_SRC_FILES})
target_link_libraries(tests gtest_main memorymanager)
```
