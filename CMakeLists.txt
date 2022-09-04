########### NEW VERSION

cmake_minimum_required(VERSION 3.1)

# set the project name
project("Final project of SPM")

# what C++ standard library to be used
set(CMAKE_CXX_STANDARD 20)

# compile option for each source file
add_compile_options(-O3) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(SOURCE_FILES main.cpp)
add_executable( main ${SOURCE_FILES})

target_include_directories(main PRIVATE src/)
target_link_libraries( main ${OpenCV_LIBS} )