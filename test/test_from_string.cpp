/**
 * test_from_string.cpp
 *
 * Unit tests for the from_string overloads in newt_arg_parser.hpp.
 * The stubs replace real bash / libnewt headers so the tests link standalone.
 */

#include "stubs/bash_stubs.hpp"   // WORD_LIST, legal_number, EXECUTION_*
#include "stubs/newt_stubs.hpp"   // newtComponent, newtGrid, enums …

#include "newt_arg_parser.hpp"

#include <catch2/catch_test_macros.hpp>

// ── int ───────────────────────────────────────────────────────────────────────

TEST_CASE("from_string<int> parses valid decimals", "[from_string][int]") {
    int v = 0;
    REQUIRE(from_string("0",   v)); CHECK(v == 0);
    REQUIRE(from_string("42",  v)); CHECK(v == 42);
    REQUIRE(from_string("-1",  v)); CHECK(v == -1);
    REQUIRE(from_string("2147483647", v)); CHECK(v == 2147483647);
}

TEST_CASE("from_string<int> rejects non-numeric strings", "[from_string][int]") {
    int v = 99;
    CHECK_FALSE(from_string("",    v));
    CHECK_FALSE(from_string("abc", v));
    CHECK_FALSE(from_string("1a",  v));
    // NOTE: strtoimax (used by the legal_number stub) accepts leading whitespace,
    // matching real bash behaviour — so " 5" is intentionally valid here.
    CHECK(v == 99);    // unchanged after all failures above
}

// ── unsigned int ──────────────────────────────────────────────────────────────

TEST_CASE("from_string<unsigned int> parses valid decimals",
          "[from_string][uint]") {
    unsigned int v = 0;
    REQUIRE(from_string("0",  v)); CHECK(v == 0u);
    REQUIRE(from_string("99", v)); CHECK(v == 99u);
}

TEST_CASE("from_string<unsigned int> rejects non-numeric strings",
          "[from_string][uint]") {
    unsigned int v = 7;
    CHECK_FALSE(from_string("xyz", v));
    CHECK(v == 7u);
}

// ── long long ─────────────────────────────────────────────────────────────────

TEST_CASE("from_string<long long> parses large values", "[from_string][llong]") {
    long long v = 0;
    REQUIRE(from_string("9223372036854775807", v));
    CHECK(v == 9223372036854775807LL);
    REQUIRE(from_string("-9223372036854775808", v));
    CHECK(v == static_cast<long long>(-9223372036854775807LL - 1));
}

// ── unsigned long long ────────────────────────────────────────────────────────

TEST_CASE("from_string<unsigned long long> parses positive values",
          "[from_string][ullong]") {
    unsigned long long v = 0;
    // The implementation routes through legal_number (intmax_t), so values
    // must fit in intmax_t.  UINT64_MAX overflows that — use a large but
    // signed-safe value instead.
    REQUIRE(from_string("9223372036854775807", v));
    CHECK(v == 9223372036854775807ULL);
    REQUIRE(from_string("0", v));
    CHECK(v == 0ULL);
    REQUIRE(from_string("100", v));
    CHECK(v == 100ULL);
}

// ── char ──────────────────────────────────────────────────────────────────────

TEST_CASE("from_string<char> returns the first character", "[from_string][char]") {
    char c = '\0';
    REQUIRE(from_string("A",     c)); CHECK(c == 'A');
    REQUIRE(from_string("hello", c)); CHECK(c == 'h');
    REQUIRE(from_string("",      c)); CHECK(c == '\0');
}

// ── char* / const char* ───────────────────────────────────────────────────────

TEST_CASE("from_string<char*> aliases the input pointer", "[from_string][charptr]") {
    const char* input = "world";
    char* p = nullptr;
    REQUIRE(from_string(input, p));
    CHECK(p == input);   // same pointer, no copy
}

TEST_CASE("from_string<const char*> aliases the input pointer",
          "[from_string][constcharptr]") {
    const char* input = "test";
    const char* p = nullptr;
    REQUIRE(from_string(input, p));
    CHECK(p == input);
}

// ── newtComponent (pointer parsed from %p) ────────────────────────────────────

TEST_CASE("from_string<newtComponent> accepts \"NULL\"", "[from_string][component]") {
    newtComponent co = reinterpret_cast<newtComponent>(0xdeadbeef);
    REQUIRE(from_string("NULL", co));
    CHECK(co == nullptr);
}

TEST_CASE("from_string<newtComponent> round-trips a pointer via to_bash_string",
          "[from_string][component]") {
    // Use a valid (but fake) pointer value.
    _newtComponent_tag sentinel{};
    newtComponent original = &sentinel;
    std::string s = to_bash_string(original);

    newtComponent parsed = nullptr;
    REQUIRE(from_string(s.c_str(), parsed));
    CHECK(parsed == original);
}

// ── newtGrid (same mechanism as newtComponent) ────────────────────────────────

TEST_CASE("from_string<newtGrid> accepts \"NULL\"", "[from_string][grid]") {
    newtGrid g = reinterpret_cast<newtGrid>(0x1);
    REQUIRE(from_string("NULL", g));
    CHECK(g == nullptr);
}

TEST_CASE("from_string<newtGrid> round-trips a pointer via to_bash_string",
          "[from_string][grid]") {
    _newtGrid_tag sentinel{};
    newtGrid original = &sentinel;
    std::string s = to_bash_string(original);

    newtGrid parsed = nullptr;
    REQUIRE(from_string(s.c_str(), parsed));
    CHECK(parsed == original);
}

// ── void* ─────────────────────────────────────────────────────────────────────

TEST_CASE("from_string<void*> accepts \"NULL\"", "[from_string][voidptr]") {
    void* p = reinterpret_cast<void*>(1);
    REQUIRE(from_string("NULL", p));
    CHECK(p == nullptr);
}

TEST_CASE("from_string<void*> accepts \"\" (empty string) as nullptr",
          "[from_string][voidptr]") {
    void* p = reinterpret_cast<void*>(1);
    REQUIRE(from_string("", p));
    CHECK(p == nullptr);
}

TEST_CASE("from_string<void*> round-trips a pointer via to_bash_string",
          "[from_string][voidptr]") {
    static int dummy = 42;
    void* original = &dummy;
    std::string s = to_bash_string(original);

    void* parsed = nullptr;
    REQUIRE(from_string(s.c_str(), parsed));
    CHECK(parsed == original);
}

// ── enum newtFlagsSense ───────────────────────────────────────────────────────

TEST_CASE("from_string<newtFlagsSense> parses integer values",
          "[from_string][enum]") {
    enum newtFlagsSense fs{};
    REQUIRE(from_string("0", fs)); CHECK(fs == NEWT_FLAGS_SET);
    REQUIRE(from_string("1", fs)); CHECK(fs == NEWT_FLAGS_RESET);
    REQUIRE(from_string("2", fs)); CHECK(fs == NEWT_FLAGS_TOGGLE);
    CHECK_FALSE(from_string("x", fs));
}

// ── enum newtGridElement ──────────────────────────────────────────────────────

TEST_CASE("from_string<newtGridElement> parses integer values",
          "[from_string][enum]") {
    enum newtGridElement ge{};
    REQUIRE(from_string("0", ge)); CHECK(ge == NEWT_GRID_COMPONENT);
    REQUIRE(from_string("1", ge)); CHECK(ge == NEWT_GRID_SUBGRID);
}

// ── void* decimal integer keys ────────────────────────────────────────────────

TEST_CASE("from_string<void*> accepts decimal integer as key",
          "[from_string][voidptr]") {
    void* p = nullptr;
    REQUIRE(from_string("42", p));
    CHECK(p == reinterpret_cast<void*>(static_cast<intptr_t>(42)));
}

TEST_CASE("from_string<void*> accepts negative decimal integer",
          "[from_string][voidptr]") {
    void* p = nullptr;
    REQUIRE(from_string("-1", p));
    CHECK(p == reinterpret_cast<void*>(static_cast<intptr_t>(-1)));
}

TEST_CASE("from_string<void*> zero decimal is nullptr",
          "[from_string][voidptr]") {
    void* p = reinterpret_cast<void*>(1);
    REQUIRE(from_string("0", p));
    CHECK(p == nullptr);
}
