cmake_minimum_required(VERSION 3.10)
project(newt VERSION 0.1 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Option to specify bash headers location
set(BASH_HEADERS "/usr/include" CACHE PATH "The prefix where the bash headers are installed")

# Check for bash headers
find_path(BASH_HEADERS_FOUND builtins.h
    HINTS "${BASH_HEADERS}/bash" "${BASH_HEADERS}" "${BASH_HEADERS}/include"
    NO_DEFAULT_PATH
)

if(NOT BASH_HEADERS_FOUND)
    message(FATAL_ERROR "Unable to find the bash headers. Please use -DBASH_HEADERS=/path/to/headers/")
endif()

message(STATUS "Found bash headers at: ${BASH_HEADERS_FOUND}")

# Add subdirectories
add_subdirectory(src)

# Tests (optional — only built when BUILD_TESTING is ON, which is the default)
option(BUILD_TESTING "Build the unit tests" ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()
