/**
 * test_call_newt.cpp
 *
 * Unit tests for the call_newt template (both void and non-void overloads) in
 * newt_arg_parser.hpp.
 *
 * We use small, self-contained "fake" functions instead of real libnewt calls
 * so these tests run without a display or the libnewt library.
 */

#include "stubs/bash_stubs.hpp"
#include "stubs/newt_stubs.hpp"
#include "stubs/word_list_builder.hpp"

#include "newt_arg_parser.hpp"

#include <catch2/catch_test_macros.hpp>
#include <string>

// ── fake functions used as targets ───────────────────────────────────────────

static void fake_void_noop() {}
static void fake_void_1i(int /*a*/) {}
static void fake_void_2i(int /*a*/, int /*b*/) {}

static int  fake_ret_42()           { return 42; }
static int  fake_add(int a, int b)  { return a + b; }
static const char* fake_greet(const char* name) { return name; }

// ── void / no args ────────────────────────────────────────────────────────────

TEST_CASE("call_newt void, no args → EXECUTION_SUCCESS", "[call_newt][void]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("noop", "", fake_void_noop, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}

// ── void / correct number of args ────────────────────────────────────────────

TEST_CASE("call_newt void, one int → EXECUTION_SUCCESS", "[call_newt][void]") {
    WordListBuilder wl{"cmd", "5"};
    CHECK(call_newt("f", "a", fake_void_1i, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}

TEST_CASE("call_newt void, two ints → EXECUTION_SUCCESS", "[call_newt][void]") {
    WordListBuilder wl{"cmd", "3", "7"};
    CHECK(call_newt("f", "a b", fake_void_2i, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}

// ── void / too few args → EXECUTION_FAILURE ──────────────────────────────────

TEST_CASE("call_newt void, too few args → EXECUTION_FAILURE", "[call_newt][void]") {
    WordListBuilder wl{"cmd"};   // no real args
    CHECK(call_newt("f", "a b", fake_void_2i, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ── non-void / no varname ─────────────────────────────────────────────────────

TEST_CASE("call_newt non-void, varname nullptr → no binding, returns SUCCESS",
          "[call_newt][nonvoid]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("f", "", fake_ret_42, nullptr, wl.head())
          == EXECUTION_SUCCESS);
    CHECK(bound_vars().empty());   // nothing was bound
}

// ── non-void / with varname ───────────────────────────────────────────────────

TEST_CASE("call_newt non-void, result bound to varname", "[call_newt][nonvoid]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd"};
    char varname[] = "RESULT";
    CHECK(call_newt("f", "", fake_ret_42, varname, wl.head())
          == EXECUTION_SUCCESS);

    const std::string* val = last_bound("RESULT");
    REQUIRE(val != nullptr);
    CHECK(*val == "42");
}

TEST_CASE("call_newt add: result bound correctly", "[call_newt][nonvoid]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "10", "32"};
    char varname[] = "SUM";
    CHECK(call_newt("add", "a b", fake_add, varname, wl.head())
          == EXECUTION_SUCCESS);

    const std::string* val = last_bound("SUM");
    REQUIRE(val != nullptr);
    CHECK(*val == "42");
}

TEST_CASE("call_newt const char* return bound as string", "[call_newt][nonvoid]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "Alice"};
    char varname[] = "GREETING";
    CHECK(call_newt("greet", "name", fake_greet, varname, wl.head())
          == EXECUTION_SUCCESS);

    const std::string* val = last_bound("GREETING");
    REQUIRE(val != nullptr);
    CHECK(*val == "Alice");
}

// ── non-void / too few args → EXECUTION_FAILURE ──────────────────────────────

TEST_CASE("call_newt non-void, too few args → EXECUTION_FAILURE",
          "[call_newt][nonvoid]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "5"};   // fake_add needs two ints
    char varname[] = "X";
    CHECK(call_newt("add", "a b", fake_add, varname, wl.head())
          == EXECUTION_FAILURE);
    CHECK(bound_vars().empty());   // bind_variable must NOT have been called
}

// ── non-void / bad arg type → EXECUTION_FAILURE ──────────────────────────────

TEST_CASE("call_newt non-void, unparseable arg → EXECUTION_FAILURE",
          "[call_newt][nonvoid]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "notanint", "2"};
    char varname[] = "X";
    CHECK(call_newt("add", "a b", fake_add, varname, wl.head())
          == EXECUTION_FAILURE);
}
