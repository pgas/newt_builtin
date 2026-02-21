/**
 * newt_arg_parser.hpp
 *
 * Modern C++17 template infrastructure for writing bash builtin wrappers around
 * libnewt with minimal boilerplate.
 *
 * Key ideas (from the design discussion):
 *  1. `from_string` overloads replace the C name-mangled string_to_TYPE functions.
 *  2. `to_bash_string` overloads return std::string, eliminating xmalloc/xfree.
 *  3. `parse_args` walks a WORD_LIST and fills a std::tuple via a C++17 fold
 *     expression — each element is parsed by the matching `from_string` overload.
 *  4. `call_newt` deduces all parameter and return types directly from the
 *     function pointer, then calls parse_args + std::apply.  Two overloads cover
 *     void and non-void returns.
 *
 * Usage:
 *   int bash_newtOpenWindow(char* varname, WORD_LIST* args) {
 *       return call_newt("OpenWindow", "left top width height title",
 *                        newtOpenWindow, varname, args);
 *   }
 */

#pragma once

#include <tuple>
#include <utility>
#include <string>
#include <optional>
#include <cstdio>
#include <cstring>
#include <cstdlib>

// NOTE: The caller must include the bash headers and <newt.h> before this file:
//   extern "C" { #include <newt.h> }
//   extern "C" { #include "builtins.h"; #include "shell.h"; ... }

// ─── from_string overloads ────────────────────────────────────────────────────
// Each overload parses a C string into a typed output reference.
// Returns true on success, false on parse failure.

inline bool from_string(const char* s, int& out) {
    intmax_t i;
    if (legal_number(s, &i)) { out = static_cast<int>(i); return true; }
    return false;
}

inline bool from_string(const char* s, unsigned int& out) {
    intmax_t i;
    if (legal_number(s, &i)) { out = static_cast<unsigned int>(i); return true; }
    return false;
}

inline bool from_string(const char* s, long long& out) {
    intmax_t i;
    if (legal_number(s, &i)) { out = static_cast<long long>(i); return true; }
    return false;
}

inline bool from_string(const char* s, unsigned long long& out) {
    intmax_t i;
    if (legal_number(s, &i)) { out = static_cast<unsigned long long>(i); return true; }
    return false;
}

// char* — just alias the bash string directly (no copy needed)
inline bool from_string(const char* s, char*& out) {
    out = const_cast<char*>(s);
    return true;
}

// const char* — many libnewt functions take const char*
inline bool from_string(const char* s, const char*& out) {
    out = s;
    return true;
}

// single char — take the first character
inline bool from_string(const char* s, char& out) {
    out = (s && s[0]) ? s[0] : '\0';
    return true;
}

// newtComponent — stored as a pointer, serialised as "%p"; also accepts "NULL"
inline bool from_string(const char* s, newtComponent& out) {
    if (std::strcmp(s, "NULL") == 0) { out = nullptr; return true; }
    void* p = nullptr;
    if (std::sscanf(s, "%p", &p) == 1) {
        out = static_cast<newtComponent>(p);
        return true;
    }
    return false;
}

// newtGrid — also a pointer type; also accepts "NULL"
inline bool from_string(const char* s, newtGrid& out) {
    if (std::strcmp(s, "NULL") == 0) { out = nullptr; return true; }
    void* p = nullptr;
    if (std::sscanf(s, "%p", &p) == 1) {
        out = static_cast<newtGrid>(p);
        return true;
    }
    return false;
}

// void* — generic pointer; also accepts "NULL"
inline bool from_string(const char* s, void*& out) {
    if (std::strcmp(s, "NULL") == 0) { out = nullptr; return true; }
    return std::sscanf(s, "%p", &out) == 1;
}

// newtCallback — typedef void (*newtCallback)(newtComponent, void *)
// Passed as an opaque pointer value serialised with %p.
inline bool from_string(const char* s, newtCallback& out) {
    void* p = nullptr;
    if (std::sscanf(s, "%p", &p) == 1) {
        out = reinterpret_cast<newtCallback>(p);
        return true;
    }
    return false;
}

// newtSuspendCallback — typedef void (*newtSuspendCallback)(void *)
inline bool from_string(const char* s, newtSuspendCallback& out) {
    void* p = nullptr;
    if (std::sscanf(s, "%p", &p) == 1) {
        out = reinterpret_cast<newtSuspendCallback>(p);
        return true;
    }
    return false;
}

// newtEntryFilter — typedef char (*newtEntryFilter)(...)
inline bool from_string(const char* s, newtEntryFilter& out) {
    void* p = nullptr;
    if (std::sscanf(s, "%p", &p) == 1) {
        out = reinterpret_cast<newtEntryFilter>(p);
        return true;
    }
    return false;
}

// Enum types — parse as int then cast.
inline bool from_string(const char* s, enum newtFlagsSense& out) {
    intmax_t i;
    if (legal_number(s, &i)) { out = static_cast<enum newtFlagsSense>(i); return true; }
    return false;
}

inline bool from_string(const char* s, enum newtGridElement& out) {
    intmax_t i;
    if (legal_number(s, &i)) { out = static_cast<enum newtGridElement>(i); return true; }
    return false;
}

// ─── to_bash_string overloads ─────────────────────────────────────────────────
// Convert a libnewt return value to a std::string for binding to a bash variable.
// Using "to_bash_string" instead of "to_string" to avoid clashing with std::.

inline std::string to_bash_string(int value)               { return std::to_string(value); }
inline std::string to_bash_string(long long value)         { return std::to_string(value); }
inline std::string to_bash_string(unsigned long long value){ return std::to_string(value); }
inline std::string to_bash_string(char value)              { return std::string(1, value); }
inline std::string to_bash_string(char* value)             { return value ? value : ""; }
inline std::string to_bash_string(const char* value)       { return value ? value : ""; }

inline std::string to_bash_string(newtComponent value) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%p", static_cast<void*>(value));
    return buf;
}

inline std::string to_bash_string(newtGrid value) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%p", static_cast<void*>(value));
    return buf;
}

inline std::string to_bash_string(void* value) {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%p", value);
    return buf;
}

// ─── arg list walker ──────────────────────────────────────────────────────────

// Implementation helper: walks args->next for each index in the pack.
// C++17 fold expression with && short-circuits on first failure.
template<typename Tuple, std::size_t... Is>
bool parse_args_impl(WORD_LIST*& args, Tuple& tup, std::index_sequence<Is...>) {
    return ((args->next != nullptr
               ? (args = args->next,
                  from_string(args->word->word, std::get<Is>(tup)))
               : false) && ...);
}

// Public interface: parse exactly sizeof...(Args) arguments from the WORD_LIST.
template<typename... Args>
bool parse_args(WORD_LIST*& args, std::tuple<Args...>& tup) {
    return parse_args_impl(args, tup, std::index_sequence_for<Args...>{});
}

// Zero-argument specialisation — always succeeds (nothing to parse).
inline bool parse_args(WORD_LIST*&, std::tuple<>&) {
    return true;
}

// ─── call_newt — non-void return ──────────────────────────────────────────────
// Deduces Ret and Args... from the function pointer.  Parses arguments, calls
// the function via std::apply, and (if varname is non-null) binds the result
// to a bash variable using to_bash_string.

template<typename Ret, typename... Args>
int call_newt(const char* name, const char* usage_str,
              Ret(*fn)(Args...), char* varname, WORD_LIST* args)
{
    std::tuple<std::decay_t<Args>...> parsed;
    if (!parse_args(args, parsed))
        goto usage;

    {
        Ret rv = std::apply(fn, parsed);
        if (varname) {
            std::string value = to_bash_string(rv);
            bind_variable(varname, const_cast<char*>(value.c_str()), 0);
        }
    }
    return EXECUTION_SUCCESS;

usage:
    std::fprintf(stderr, "newt: usage: newt %s %s\n", name, usage_str);
    return EXECUTION_FAILURE;
}

// ─── call_newt — void return ──────────────────────────────────────────────────

template<typename... Args>
int call_newt(const char* name, const char* usage_str,
              void(*fn)(Args...), char* /*varname*/, WORD_LIST* args)
{
    std::tuple<std::decay_t<Args>...> parsed;
    if (!parse_args(args, parsed))
        goto usage;

    std::apply(fn, parsed);
    return EXECUTION_SUCCESS;

usage:
    std::fprintf(stderr, "newt: usage: newt %s %s\n", name, usage_str);
    return EXECUTION_FAILURE;
}
