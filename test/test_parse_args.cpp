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
