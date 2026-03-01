# tests/CMakeLists.txt
# Unit tests for newt_arg_parser.hpp using Catch2 (fetched automatically).
#
# The tests compile newt_arg_parser.hpp against lightweight stubs that replace
# real bash / libnewt headers, so no display or bash installation is needed.

cmake_minimum_required(VERSION 3.14)

# ── Catch2 via FetchContent ───────────────────────────────────────────────────
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

# ── test executable ───────────────────────────────────────────────────────────
add_executable(newt_tests
    test_from_string.cpp
    test_to_bash_string.cpp
    test_parse_args.cpp
    test_call_newt.cpp
    test_components.cpp
    test_wrappers.cpp
    test_new_wrappers.cpp
)

target_compile_features(newt_tests PRIVATE cxx_std_17)

target_include_directories(newt_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"           # for stubs/ subdirectory
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"    # for newt_arg_parser.hpp
)

# Catch2WithMain provides its own main() — no need to write one.
target_link_libraries(newt_tests PRIVATE Catch2::Catch2WithMain)

# ── CTest integration ─────────────────────────────────────────────────────────
include(CTest)
include(Catch)
catch_discover_tests(newt_tests)
