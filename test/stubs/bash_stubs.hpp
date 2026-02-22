/**
 * bash_stubs.hpp
 *
 * Minimal stubs for the bash types and functions used by newt_arg_parser.hpp,
 * so we can compile and test that header without a real bash installation.
 */
#pragma once

#include <cerrno>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

// ── WORD_DESC / WORD_LIST ─────────────────────────────────────────────────────

struct word_desc {
    char* word;
    int   flags;
};
using WORD_DESC = word_desc;

struct word_list {
    word_list* next;
    WORD_DESC* word;
};
using WORD_LIST = word_list;

// ── SHELL_VAR — opaque stub (only pointer used by builtin_bind_variable) ──────
struct SHELL_VAR {};

// ── constants ─────────────────────────────────────────────────────────────────

static constexpr int EXECUTION_SUCCESS = 0;
static constexpr int EXECUTION_FAILURE = 1;

// ── legal_number ──────────────────────────────────────────────────────────────
// Parses a decimal integer string.  Returns 1 on success, 0 on failure.
inline int legal_number(const char* s, intmax_t* result) {
    if (!s || !*s) return 0;
    char* end = nullptr;
    errno = 0;
    intmax_t v = std::strtoimax(s, &end, 10);
    if (errno != 0 || end == s || *end != '\0') return 0;
    *result = v;
    return 1;
}

// ── bind_variable ─────────────────────────────────────────────────────────────
// Records every (name, value) pair so tests can inspect what was bound.

struct BoundVar {
    std::string name;
    std::string value;
};

// Global store — clear between tests with clear_bound_vars().
inline std::vector<BoundVar>& bound_vars() {
    static std::vector<BoundVar> store;
    return store;
}

inline int bind_variable(const char* name, char* value, int /*flags*/) {
    bound_vars().push_back({name ? name : "", value ? value : ""});
    return 0;
}

// builtin_bind_variable — handles array references (used by production code)
inline SHELL_VAR* builtin_bind_variable(const char* name, const char* value, int flags) {
    bound_vars().push_back({name ? name : "", value ? value : ""});
    return nullptr;
}

// ── test helpers ──────────────────────────────────────────────────────────────

inline void clear_bound_vars() { bound_vars().clear(); }

// Returns the value most recently bound to 'name', or nullptr if not found.
inline const std::string* last_bound(const std::string& name) {
    auto& v = bound_vars();
    for (auto it = v.rbegin(); it != v.rend(); ++it)
        if (it->name == name) return &it->value;
    return nullptr;
}

// xmalloc / xfree stubs used by a few wrappers (not by the parser layer, but
// included here for completeness).
inline void* xmalloc(std::size_t n) { return std::malloc(n); }
inline void  xfree(void* p)         { std::free(p); }
