cmake_minimum_required(VERSION 3.0)
project(swurf)

## Where are the additional libraries installed?
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

## If no external build type is set, set it to release.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE release)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

## The compiler flags for compiling C sources.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wconversion")
## The compiler flags for compiling C++ sources.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wconversion")

message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

##set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

## Set this to see the full make commands.
if(NOT CMAKE_VERBOSE_MAKEFILE)
  set(CMAKE_VERBOSE_MAKEFILE OFF)
endif()
message(STATUS "CMAKE_VERBOSE_MAKEFILE: ${CMAKE_VERBOSE_MAKEFILE}")

## Make sure necessary libs are installed.
find_package(Threads REQUIRED)
find_package(MHD REQUIRED)
find_package(GnuTLS)
message(STATUS "MHD lib: ${MHD_LIBRARIES}")

## Directory paths
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

## Look for sources
file(GLOB SRCS ${SRC_DIR}/*.cpp)

## Define library.
add_library(swurf ${SRCS})

## Include header files.
target_include_directories(swurf PUBLIC ${INC_DIR})

## Link external libs.
target_link_libraries(swurf ${MHD_LIBRARIES})
target_link_libraries(swurf ${GNUTLS_LIBRARIES})

## Directory paths for application
set(HELLO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples/hello-world)
file(GLOB HELLO_SRCS ${HELLO_SRC_DIR}/*.cpp)

## Define application
add_executable(hello ${HELLO_SRCS})

## Link external libs for application
target_include_directories(hello PUBLIC ${FIP_INC_DIR})
target_link_libraries(hello swurf)
target_link_libraries(hello ${CMAKE_THREAD_LIBS_INIT})
