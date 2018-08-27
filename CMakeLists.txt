cmake_minimum_required(VERSION 3.9)
project(downsample_impl)

# Do not allow in-source builds:
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are not allowed.")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FindThreads)
find_package(OpenMP 3.1)
if (OPENMP_FOUND)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

find_package(PythonLibs 3.6)
include_directories(${PYTHON_INCLUDE_DIRS})

find_package(Boost 1.65
    COMPONENTS
		python3
		numpy3
    REQUIRED)
message(STATUS "Boost includes: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libs: ${Boost_LIBRARIES}")
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE SRCS downsample/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
target_link_libraries(${PROJECT_NAME}
    ${Boost_LIBRARIES}
	${PYTHON_LIBRARIES}
	Threads::Threads)
