/**
 * test_to_bash_string.cpp
 *
 * Unit tests for the to_bash_string overloads in newt_arg_parser.hpp.
 */

#include "stubs/bash_stubs.hpp"
#include "stubs/newt_stubs.hpp"

#include "newt_arg_parser.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <cstring>

// ── int ───────────────────────────────────────────────────────────────────────

TEST_CASE("to_bash_string(int) produces decimal string", "[to_bash_string][int]") {
    CHECK(to_bash_string(0)    == "0");
    CHECK(to_bash_string(42)   == "42");
    CHECK(to_bash_string(-1)   == "-1");
    CHECK(to_bash_string(2147483647) == "2147483647");
}

// ── long long ─────────────────────────────────────────────────────────────────

TEST_CASE("to_bash_string(long long) produces decimal string",
          "[to_bash_string][llong]") {
    CHECK(to_bash_string(0LL)  == "0");
    CHECK(to_bash_string(-1LL) == "-1");
    CHECK(to_bash_string(9223372036854775807LL) == "9223372036854775807");
}

// ── unsigned long long ────────────────────────────────────────────────────────

TEST_CASE("to_bash_string(unsigned long long) produces decimal string",
          "[to_bash_string][ullong]") {
    CHECK(to_bash_string(0ULL)  == "0");
    CHECK(to_bash_string(18446744073709551615ULL) == "18446744073709551615");
}

// ── char ──────────────────────────────────────────────────────────────────────

TEST_CASE("to_bash_string(char) produces a single-character string",
          "[to_bash_string][char]") {
    CHECK(to_bash_string('A') == "A");
    CHECK(to_bash_string('z') == "z");
    CHECK(to_bash_string('0') == "0");
}

// ── char* / const char* ───────────────────────────────────────────────────────

TEST_CASE("to_bash_string(char*) returns the pointed-to string",
          "[to_bash_string][charptr]") {
    CHECK(to_bash_string((char*)"hello") == "hello");
    CHECK(to_bash_string((char*)"")      == "");
    CHECK(to_bash_string((char*)nullptr) == "");
}

TEST_CASE("to_bash_string(const char*) returns the pointed-to string",
          "[to_bash_string][constcharptr]") {
    CHECK(to_bash_string((const char*)"world") == "world");
    CHECK(to_bash_string((const char*)nullptr) == "");
}

// ── newtComponent pointer ────────────────────────────────────────────────────

TEST_CASE("to_bash_string(newtComponent nullptr) produces a non-empty string",
          "[to_bash_string][component]") {
    // The exact representation is platform-defined (%p), but it must be
    // non-empty and parseable back to nullptr via from_string.
    std::string s = to_bash_string((newtComponent) nullptr);
    REQUIRE_FALSE(s.empty());

    newtComponent roundtrip = reinterpret_cast<newtComponent>(0x1); // non-null
    REQUIRE(from_string(s.c_str(), roundtrip));
    CHECK(roundtrip == nullptr);
}

TEST_CASE("to_bash_string(newtComponent) round-trips through from_string",
          "[to_bash_string][component]") {
    _newtComponent_tag sentinel{};
    newtComponent original = &sentinel;

    std::string s = to_bash_string(original);
    newtComponent parsed = nullptr;
    REQUIRE(from_string(s.c_str(), parsed));
    CHECK(parsed == original);
}

// ── newtGrid pointer ──────────────────────────────────────────────────────────

TEST_CASE("to_bash_string(newtGrid) round-trips through from_string",
          "[to_bash_string][grid]") {
    _newtGrid_tag sentinel{};
    newtGrid original = &sentinel;

    std::string s = to_bash_string(original);
    newtGrid parsed = nullptr;
    REQUIRE(from_string(s.c_str(), parsed));
    CHECK(parsed == original);
}

// ── void* ─────────────────────────────────────────────────────────────────────

TEST_CASE("to_bash_string(void*) round-trips through from_string",
          "[to_bash_string][voidptr]") {
    static int dummy = 0;
    void* original = &dummy;

    std::string s = to_bash_string(original);
    void* parsed = nullptr;
    REQUIRE(from_string(s.c_str(), parsed));
    CHECK(parsed == original);
}
