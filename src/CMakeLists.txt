# Build newt.so shared library (Bash builtin module)

# Get the bash headers directory
find_path(BASH_HEADERS_FOUND builtins.h
    HINTS "${BASH_HEADERS}/bash" "${BASH_HEADERS}" "${BASH_HEADERS}/include"
    NO_DEFAULT_PATH
)

if(NOT BASH_HEADERS_FOUND)
    message(FATAL_ERROR "Unable to find the bash headers. Please use -DBASH_HEADERS=/path/to/headers/")
endif()

# Create the shared library
add_library(newt MODULE
    newt.cpp
    newt_wrappers.cpp
    newt_constants.cpp
    newt_arg_parser.hpp
    newt_wrappers.hpp
    newt_constants.hpp
)

# Set library properties
set_target_properties(newt PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)

# Link against libnewt
find_library(NEWT_LIB newt REQUIRED)
target_link_libraries(newt PRIVATE ${NEWT_LIB})

# Add include directories for bash headers
target_include_directories(newt PRIVATE
    "${BASH_HEADERS}/bash"
    "${BASH_HEADERS}/bash/include"
    "${BASH_HEADERS}/bash/builtins"
    "${BASH_HEADERS}"
    "${BASH_HEADERS}/include"
    "${BASH_HEADERS}/builtins"
)

# Install the shared library to lib/bash
install(TARGETS newt
    LIBRARY DESTINATION lib/bash
)
