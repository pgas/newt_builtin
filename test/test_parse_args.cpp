/**
 * test_parse_args.cpp
 *
 * Unit tests for the parse_args template in newt_arg_parser.hpp.
 *
 * parse_args walks a WORD_LIST (the first node is a "command" sentinel; real
 * arguments start at node->next) and fills a std::tuple with typed values via
 * from_string.  It returns true on success and false when there are too few
 * arguments.
 */

#include "stubs/bash_stubs.hpp"
#include "stubs/newt_stubs.hpp"
#include "stubs/word_list_builder.hpp"

#include "newt_arg_parser.hpp"

#include <catch2/catch_test_macros.hpp>
#include <tuple>

// ── zero arguments ────────────────────────────────────────────────────────────

TEST_CASE("parse_args succeeds with empty tuple (zero args)", "[parse_args]") {
    WordListBuilder wl{"cmd"};
    WORD_LIST* args = wl.head();
    std::tuple<> tup{};
    REQUIRE(parse_args(args, tup));
}

// ── single argument ───────────────────────────────────────────────────────────

TEST_CASE("parse_args parses a single int", "[parse_args][int]") {
    WordListBuilder wl{"cmd", "42"};
    WORD_LIST* args = wl.head();
    std::tuple<int> tup{};
    REQUIRE(parse_args(args, tup));
    CHECK(std::get<0>(tup) == 42);
}

TEST_CASE("parse_args parses a single const char*", "[parse_args][charptr]") {
    WordListBuilder wl{"cmd", "hello"};
    WORD_LIST* args = wl.head();
    std::tuple<const char*> tup{};
    REQUIRE(parse_args(args, tup));
    CHECK(std::string(std::get<0>(tup)) == "hello");
}

// ── multiple arguments ────────────────────────────────────────────────────────

TEST_CASE("parse_args parses (int, const char*, int)", "[parse_args][multi]") {
    WordListBuilder wl{"cmd", "10", "world", "99"};
    WORD_LIST* args = wl.head();
    std::tuple<int, const char*, int> tup{};
    REQUIRE(parse_args(args, tup));
    CHECK(std::get<0>(tup) == 10);
    CHECK(std::string(std::get<1>(tup)) == "world");
    CHECK(std::get<2>(tup) == 99);
}

TEST_CASE("parse_args parses (int, int, int, int) — four arguments",
          "[parse_args][multi]") {
    WordListBuilder wl{"cmd", "1", "2", "3", "4"};
    WORD_LIST* args = wl.head();
    std::tuple<int, int, int, int> tup{};
    REQUIRE(parse_args(args, tup));
    CHECK(std::get<0>(tup) == 1);
    CHECK(std::get<1>(tup) == 2);
    CHECK(std::get<2>(tup) == 3);
    CHECK(std::get<3>(tup) == 4);
}

// ── too few arguments → failure ───────────────────────────────────────────────

TEST_CASE("parse_args returns false when args list is shorter than tuple",
          "[parse_args][underflow]") {
    // Tuple expects two ints but we supply only one.
    WordListBuilder wl{"cmd", "5"};
    WORD_LIST* args = wl.head();
    std::tuple<int, int> tup{};
    CHECK_FALSE(parse_args(args, tup));
}

TEST_CASE("parse_args returns false when arg list is empty (sentinel only)",
          "[parse_args][underflow]") {
    WordListBuilder wl{"cmd"};
    WORD_LIST* args = wl.head();
    std::tuple<int> tup{};
    CHECK_FALSE(parse_args(args, tup));
}

// ── extra (unconsumed) arguments do not cause failure ─────────────────────────

TEST_CASE("parse_args consumes exactly as many args as the tuple needs",
          "[parse_args][extra]") {
    // Builder provides three real args; tuple only needs one.
    WordListBuilder wl{"cmd", "7", "8", "9"};
    WORD_LIST* args = wl.head();
    std::tuple<int> tup{};
    REQUIRE(parse_args(args, tup));
    CHECK(std::get<0>(tup) == 7);
    // parse_args advances to the last consumed node ("7"); the first
    // unconsumed word is one step further at args->next.
    REQUIRE(args != nullptr);
    REQUIRE(args->next != nullptr);
    CHECK(std::string(args->next->word->word) == "8");
}

// ── from_string failure propagates to parse_args ──────────────────────────────

TEST_CASE("parse_args returns false when from_string fails for an arg",
          "[parse_args][parse_failure]") {
    // "notanumber" cannot be parsed as int.
    WordListBuilder wl{"cmd", "notanumber"};
    WORD_LIST* args = wl.head();
    std::tuple<int> tup{};
    CHECK_FALSE(parse_args(args, tup));
}

// ── 44 const char* args ── covers the SetColors field count ──────────────────

// Build a WordListBuilder with 44 colour strings for the SetColors test.
static std::vector<std::string> make_color_args() {
    // Matches the newtDefaultColorPalette order:
    // rootFg/Bg borderFg/Bg windowFg/Bg shadowFg/Bg titleFg/Bg
    // buttonFg/Bg actButtonFg/Bg checkboxFg/Bg actCheckboxFg/Bg entryFg/Bg
    // labelFg/Bg listboxFg/Bg actListboxFg/Bg textboxFg/Bg actTextboxFg/Bg
    // helpLineFg/Bg rootTextFg/Bg emptyScale fullScale
    // disabledEntryFg/Bg compactButtonFg/Bg actSelListboxFg/Bg selListboxFg/Bg
    return {
        "white","blue",    "black","blue",    "black","white",  "white","black",
        "red",  "white",   "white","red",     "white","red",    "white","black",
        "white","black",   "black","white",   "blue", "white",  "black","white",
        "white","blue",    "black","white",   "white","blue",   "black","white",
        "black","white",   "white",           "red",
        "black","white",   "white","black",   "white","blue",   "black","white",
    };
}

TEST_CASE("parse_args handles 44 const char* args (SetColors shape)",
          "[parse_args][setcolors]") {
    std::vector<std::string> tokens{"cmd"};
    auto colors = make_color_args();
    tokens.insert(tokens.end(), colors.begin(), colors.end());
    REQUIRE(tokens.size() == 45);   // sentinel + 44 color strings

    WordListBuilder wl{tokens};
    WORD_LIST* args = wl.head();

    std::tuple<
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*
    > tup{};
    REQUIRE(parse_args(args, tup));
    // Spot-check the first and last fields.
    CHECK(std::string(std::get<0>(tup))  == "white");  // rootFg
    CHECK(std::string(std::get<1>(tup))  == "blue");   // rootBg
    CHECK(std::string(std::get<42>(tup)) == "black");  // selListboxFg
    CHECK(std::string(std::get<43>(tup)) == "white");  // selListboxBg
}

TEST_CASE("parse_args fails when one arg short of 44 (SetColors shape)",
          "[parse_args][setcolors]") {
    std::vector<std::string> tokens{"cmd"};
    auto colors = make_color_args();
    colors.pop_back();  // remove the last arg → only 43 supplied
    tokens.insert(tokens.end(), colors.begin(), colors.end());

    WordListBuilder wl{tokens};
    WORD_LIST* args = wl.head();

    std::tuple<
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*, const char*, const char*, const char*, const char*,
        const char*, const char*
    > tup{};
    CHECK_FALSE(parse_args(args, tup));
}
