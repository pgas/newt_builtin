/**
 * test_new_wrappers.cpp
 *
 * Unit tests for the wrappers added in the second round of implementation:
 *   CheckboxTree family, ListboxGetSelection, TextboxReflowed, ReflowText,
 *   CreateGrid, GridSetField, GridGetSize, GridWrappedWindow/At,
 *   GridAddComponentsToForm, GridVStacked/HStacked/VCloseStacked/HCloseStacked,
 *   WinMessage, WinChoice, WinTernary, WinMenu, ButtonBar.
 *
 * Pattern:
 *   • call_newt-style wrappers: exercise call_newt() directly with a fake fn.
 *   • Hand-written wrappers: re-implement the argument-parse + bind logic with
 *     fake functions, then test success + too-few-args failure.
 */

#include "stubs/bash_stubs.hpp"
#include "stubs/newt_stubs.hpp"
#include "stubs/word_list_builder.hpp"
#include "newt_arg_parser.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

static constexpr const char* CO   = "";   // empty string → nullptr newtComponent
static constexpr const char* GRID = "";   // empty string → nullptr newtGrid

// ═════════════════════════════════════════════════════════════════════════════
// ── CheckboxTree constructors ─────────────────────────────────────────────────
// ═════════════════════════════════════════════════════════════════════════════

// newtComponent newtCheckboxTree(int left, int top, int height, int flags)
static newtComponent fake_checkbox_tree(int, int, int, int) { return nullptr; }

TEST_CASE("CheckboxTree: left top height flags → SUCCESS", "[new_wrappers][CheckboxTree]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "3", "2", "10", "0"};
    char v[] = "ct";
    CHECK(call_newt("CheckboxTree", "left top height flags",
                    fake_checkbox_tree, v, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("CheckboxTree: too few args → FAILURE", "[new_wrappers][CheckboxTree]") {
    WordListBuilder wl{"cmd", "3", "2", "10"};   // missing flags
    CHECK(call_newt("CheckboxTree", "left top height flags",
                    fake_checkbox_tree, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// newtComponent newtCheckboxTreeMulti(int left, int top, int height, char *seq, int flags)
static newtComponent fake_checkbox_tree_multi(int, int, int, char*, int) { return nullptr; }

TEST_CASE("CheckboxTreeMulti: left top height seq flags → SUCCESS",
          "[new_wrappers][CheckboxTreeMulti]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "3", "2", "10", "*x ", "0"};
    char v[] = "ct";
    CHECK(call_newt("CheckboxTreeMulti", "left top height seq flags",
                    fake_checkbox_tree_multi, v, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("CheckboxTreeMulti: too few args → FAILURE", "[new_wrappers][CheckboxTreeMulti]") {
    WordListBuilder wl{"cmd", "3", "2", "10"};   // missing seq, flags
    CHECK(call_newt("CheckboxTreeMulti", "left top height seq flags",
                    fake_checkbox_tree_multi, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── CheckboxTree setters that use call_newt ───────────────────────────────────
// ═════════════════════════════════════════════════════════════════════════════

// void newtCheckboxTreeSetCurrent(newtComponent co, void* item)
static void fake_ct_set_current(newtComponent, void*) {}

TEST_CASE("CheckboxTreeSetCurrent: co item → SUCCESS",
          "[new_wrappers][CheckboxTreeSetCurrent]") {
    WordListBuilder wl{"cmd", CO, "42"};
    CHECK(call_newt("CheckboxTreeSetCurrent", "co item",
                    fake_ct_set_current, nullptr, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("CheckboxTreeSetCurrent: too few args → FAILURE",
          "[new_wrappers][CheckboxTreeSetCurrent]") {
    WordListBuilder wl{"cmd", CO};   // missing item
    CHECK(call_newt("CheckboxTreeSetCurrent", "co item",
                    fake_ct_set_current, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// const void* newtCheckboxTreeGetCurrent(newtComponent co)
static const void* fake_ct_get_current(newtComponent) {
    return reinterpret_cast<const void*>(static_cast<intptr_t>(7));
}

TEST_CASE("CheckboxTreeGetCurrent: co → SUCCESS, value bound",
          "[new_wrappers][CheckboxTreeGetCurrent]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO};
    char v[] = "cur";
    CHECK(call_newt("CheckboxTreeGetCurrent", "co",
                    fake_ct_get_current, v, wl.head()) == EXECUTION_SUCCESS);
    const std::string* s = last_bound("cur");
    REQUIRE(s != nullptr);
    CHECK(*s == "7");
}
TEST_CASE("CheckboxTreeGetCurrent: no args → FAILURE",
          "[new_wrappers][CheckboxTreeGetCurrent]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("CheckboxTreeGetCurrent", "co",
                    fake_ct_get_current, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// void newtCheckboxTreeSetEntry(newtComponent co, const void* data, const char* text)
static void fake_ct_set_entry(newtComponent, const void*, const char*) {}

TEST_CASE("CheckboxTreeSetEntry: co data text → SUCCESS",
          "[new_wrappers][CheckboxTreeSetEntry]") {
    WordListBuilder wl{"cmd", CO, "1", "Label"};
    CHECK(call_newt("CheckboxTreeSetEntry", "co data text",
                    fake_ct_set_entry, nullptr, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("CheckboxTreeSetEntry: too few args → FAILURE",
          "[new_wrappers][CheckboxTreeSetEntry]") {
    WordListBuilder wl{"cmd", CO, "1"};   // missing text
    CHECK(call_newt("CheckboxTreeSetEntry", "co data text",
                    fake_ct_set_entry, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// void newtCheckboxTreeSetWidth(newtComponent co, int width)
static void fake_ct_set_width(newtComponent, int) {}

TEST_CASE("CheckboxTreeSetWidth: co width → SUCCESS",
          "[new_wrappers][CheckboxTreeSetWidth]") {
    WordListBuilder wl{"cmd", CO, "30"};
    CHECK(call_newt("CheckboxTreeSetWidth", "co width",
                    fake_ct_set_width, nullptr, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("CheckboxTreeSetWidth: too few args → FAILURE",
          "[new_wrappers][CheckboxTreeSetWidth]") {
    WordListBuilder wl{"cmd", CO};   // missing width
    CHECK(call_newt("CheckboxTreeSetWidth", "co width",
                    fake_ct_set_width, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// char newtCheckboxTreeGetEntryValue(newtComponent co, const void* data)
static char fake_ct_get_entry_value(newtComponent, const void*) { return '*'; }

TEST_CASE("CheckboxTreeGetEntryValue: co data → SUCCESS, '*' bound",
          "[new_wrappers][CheckboxTreeGetEntryValue]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, "1"};
    char v[] = "val";
    CHECK(call_newt("CheckboxTreeGetEntryValue", "co data",
                    fake_ct_get_entry_value, v, wl.head()) == EXECUTION_SUCCESS);
    const std::string* s = last_bound("val");
    REQUIRE(s != nullptr);
    CHECK(*s == "*");
}
TEST_CASE("CheckboxTreeGetEntryValue: too few args → FAILURE",
          "[new_wrappers][CheckboxTreeGetEntryValue]") {
    WordListBuilder wl{"cmd", CO};   // missing data
    CHECK(call_newt("CheckboxTreeGetEntryValue", "co data",
                    fake_ct_get_entry_value, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// void newtCheckboxTreeSetEntryValue(newtComponent co, const void* data, char value)
static void fake_ct_set_entry_value(newtComponent, const void*, char) {}

TEST_CASE("CheckboxTreeSetEntryValue: co data value → SUCCESS",
          "[new_wrappers][CheckboxTreeSetEntryValue]") {
    WordListBuilder wl{"cmd", CO, "1", "*"};
    CHECK(call_newt("CheckboxTreeSetEntryValue", "co data value",
                    fake_ct_set_entry_value, nullptr, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("CheckboxTreeSetEntryValue: too few args → FAILURE",
          "[new_wrappers][CheckboxTreeSetEntryValue]") {
    WordListBuilder wl{"cmd", CO, "1"};   // missing value
    CHECK(call_newt("CheckboxTreeSetEntryValue", "co data value",
                    fake_ct_set_entry_value, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── CheckboxTreeGetSelection — hand-written logic ──────────────────────────────
// ═════════════════════════════════════════════════════════════════════════════

// Provide a statically controllable fake newtCheckboxTreeGetSelection
static const void* fake_ct_sel_data[] = {
    reinterpret_cast<const void*>(static_cast<intptr_t>(10)),
    reinterpret_cast<const void*>(static_cast<intptr_t>(20)),
};
static const void** fake_ct_get_selection(newtComponent, int* n) {
    *n = 2;
    return fake_ct_sel_data;
}

static int test_wrap_CheckboxTreeGetSelection(WORD_LIST* a) {
    newtComponent co;
    const char* varname;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co))      goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, varname)) goto usage;
    {
        int n = 0;
        const void** sel = fake_ct_get_selection(co, &n);
        for (int i = 0; i < n; ++i) {
            std::string idx_var = std::string(varname) + "_" + std::to_string(i);
            std::string val     = to_bash_string(sel[i]);
            bind_variable(const_cast<char*>(idx_var.c_str()),
                          const_cast<char*>(val.c_str()), 0);
        }
        bind_variable(const_cast<char*>(varname),
                      const_cast<char*>(to_bash_string(n).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("CheckboxTreeGetSelection: co varname → binds count and items",
          "[new_wrappers][CheckboxTreeGetSelection]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, "sel"};
    REQUIRE(test_wrap_CheckboxTreeGetSelection(wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("sel")   != nullptr); CHECK(*last_bound("sel")   == "2");
    REQUIRE(last_bound("sel_0") != nullptr); CHECK(*last_bound("sel_0") == "10");
    REQUIRE(last_bound("sel_1") != nullptr); CHECK(*last_bound("sel_1") == "20");
}
TEST_CASE("CheckboxTreeGetSelection: too few args → FAILURE",
          "[new_wrappers][CheckboxTreeGetSelection]") {
    WordListBuilder wl{"cmd", CO};   // missing varname
    CHECK(test_wrap_CheckboxTreeGetSelection(wl.head()) == EXECUTION_FAILURE);
}
TEST_CASE("CheckboxTreeGetSelection: no args → FAILURE",
          "[new_wrappers][CheckboxTreeGetSelection]") {
    WordListBuilder wl{"cmd"};
    CHECK(test_wrap_CheckboxTreeGetSelection(wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── CheckboxTreeGetMultiSelection — hand-written logic ────────────────────────
// ═════════════════════════════════════════════════════════════════════════════

static const void** fake_ct_get_multi_selection(newtComponent, int* n, char) {
    *n = 1;
    static const void* d[] = { reinterpret_cast<const void*>(static_cast<intptr_t>(5)) };
    return d;
}

static int test_wrap_CheckboxTreeGetMultiSelection(WORD_LIST* a) {
    newtComponent co;
    const char* varname;
    char seqnum;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co))      goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, varname)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, seqnum))  goto usage;
    {
        int n = 0;
        const void** sel = fake_ct_get_multi_selection(co, &n, seqnum);
        for (int i = 0; i < n; ++i) {
            std::string idx_var = std::string(varname) + "_" + std::to_string(i);
            std::string val     = to_bash_string(sel[i]);
            bind_variable(const_cast<char*>(idx_var.c_str()),
                          const_cast<char*>(val.c_str()), 0);
        }
        bind_variable(const_cast<char*>(varname),
                      const_cast<char*>(to_bash_string(n).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("CheckboxTreeGetMultiSelection: co varname seqnum → binds items",
          "[new_wrappers][CheckboxTreeGetMultiSelection]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, "ms", "*"};
    REQUIRE(test_wrap_CheckboxTreeGetMultiSelection(wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("ms")   != nullptr); CHECK(*last_bound("ms")   == "1");
    REQUIRE(last_bound("ms_0") != nullptr); CHECK(*last_bound("ms_0") == "5");
}
TEST_CASE("CheckboxTreeGetMultiSelection: too few args → FAILURE",
          "[new_wrappers][CheckboxTreeGetMultiSelection]") {
    WordListBuilder wl{"cmd", CO, "ms"};   // missing seqnum
    CHECK(test_wrap_CheckboxTreeGetMultiSelection(wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── CheckboxTreeAddItem — hand-written logic ──────────────────────────────────
// Uses newtCheckboxTreeAddArray(co, text, data, flags, int*) internally.
// ═════════════════════════════════════════════════════════════════════════════

static int last_add_item_result = 0;
static int fake_ct_add_array(newtComponent, const char*, const void*, int flags, int* indexes) {
    // Count the indexes (terminated by NEWT_ARG_LAST).
    last_add_item_result = flags;
    (void)indexes;
    return 42;
}

static int test_wrap_CheckboxTreeAddItem(WORD_LIST* a) {
    newtComponent co;
    const char* text;
    void*       data_p;
    int         flags;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co))    goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, text))  goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, data_p)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, flags)) goto usage;
    {
        std::vector<int> indexes;
        while (a->next) {
            a = a->next;
            int idx;
            if (!from_string(a->word->word, idx)) goto usage;
            indexes.push_back(idx);
        }
        indexes.push_back(NEWT_ARG_LAST);
        int rc = fake_ct_add_array(co, text, data_p, flags, indexes.data());
        bind_variable(const_cast<char*>("res"),
                      const_cast<char*>(to_bash_string(rc).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("CheckboxTreeAddItem: co text data flags idx1 idx2 → SUCCESS",
          "[new_wrappers][CheckboxTreeAddItem]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, "Item", "1", "0", "0", "1"};
    REQUIRE(test_wrap_CheckboxTreeAddItem(wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("res") != nullptr);
    CHECK(*last_bound("res") == "42");
}
TEST_CASE("CheckboxTreeAddItem: too few args → FAILURE",
          "[new_wrappers][CheckboxTreeAddItem]") {
    WordListBuilder wl{"cmd", CO, "Item", "1"};  // missing flags
    CHECK(test_wrap_CheckboxTreeAddItem(wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── CheckboxTreeFindItem — hand-written logic ─────────────────────────────────
// ═════════════════════════════════════════════════════════════════════════════

static int fake_ct_find_item_result[] = {0, 1, NEWT_ARG_LAST};
static int* fake_ct_find_item(newtComponent, void*) {
    return fake_ct_find_item_result;
}

static int test_wrap_CheckboxTreeFindItem(char* v, WORD_LIST* a) {
    newtComponent co;
    void* data_p;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co))     goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, data_p)) goto usage;
    {
        int* idxs = fake_ct_find_item(co, data_p);
        std::string result;
        if (idxs) {
            for (int i = 0; idxs[i] != NEWT_ARG_LAST; ++i) {
                if (i) result += ' ';
                result += to_bash_string(idxs[i]);
            }
        }
        if (v)
            bind_variable(v, const_cast<char*>(result.c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("CheckboxTreeFindItem: co data → SUCCESS, space-separated indexes",
          "[new_wrappers][CheckboxTreeFindItem]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, "1"};
    char v[] = "path";
    REQUIRE(test_wrap_CheckboxTreeFindItem(v, wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("path") != nullptr);
    CHECK(*last_bound("path") == "0 1");
}
TEST_CASE("CheckboxTreeFindItem: too few args → FAILURE",
          "[new_wrappers][CheckboxTreeFindItem]") {
    WordListBuilder wl{"cmd", CO};   // missing data
    CHECK(test_wrap_CheckboxTreeFindItem(nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── ListboxGetSelection — hand-written logic ──────────────────────────────────
// ═════════════════════════════════════════════════════════════════════════════

static void* fake_lb_sel_data[] = {
    reinterpret_cast<void*>(static_cast<intptr_t>(100)),
    reinterpret_cast<void*>(static_cast<intptr_t>(200)),
    reinterpret_cast<void*>(static_cast<intptr_t>(300)),
};
static void** fake_lb_get_selection(newtComponent, int* n) {
    *n = 3;
    return fake_lb_sel_data;
}

static int test_wrap_ListboxGetSelection(WORD_LIST* a) {
    newtComponent co;
    const char* varname;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co))      goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, varname)) goto usage;
    {
        int n = 0;
        void** sel = fake_lb_get_selection(co, &n);
        for (int i = 0; i < n; ++i) {
            std::string idx_var = std::string(varname) + "_" + std::to_string(i);
            std::string val     = to_bash_string(sel[i]);
            bind_variable(const_cast<char*>(idx_var.c_str()),
                          const_cast<char*>(val.c_str()), 0);
        }
        bind_variable(const_cast<char*>(varname),
                      const_cast<char*>(to_bash_string(n).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("ListboxGetSelection: co varname → binds count and 3 items",
          "[new_wrappers][ListboxGetSelection]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, "lsel"};
    REQUIRE(test_wrap_ListboxGetSelection(wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("lsel")   != nullptr); CHECK(*last_bound("lsel")   == "3");
    REQUIRE(last_bound("lsel_0") != nullptr); CHECK(*last_bound("lsel_0") == "100");
    REQUIRE(last_bound("lsel_1") != nullptr); CHECK(*last_bound("lsel_1") == "200");
    REQUIRE(last_bound("lsel_2") != nullptr); CHECK(*last_bound("lsel_2") == "300");
}
TEST_CASE("ListboxGetSelection: too few args → FAILURE",
          "[new_wrappers][ListboxGetSelection]") {
    WordListBuilder wl{"cmd", CO};   // missing varname
    CHECK(test_wrap_ListboxGetSelection(wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── TextboxReflowed constructor ───────────────────────────────────────────────
// newtComponent(int left, int top, char* text, int width, int flexDown,
//               int flexUp, int flags)
// ═════════════════════════════════════════════════════════════════════════════

static newtComponent fake_textbox_reflowed(int, int, char*, int, int, int, int) {
    return nullptr;
}

TEST_CASE("TextboxReflowed: 7 args → SUCCESS",
          "[new_wrappers][TextboxReflowed]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "0", "0", "hello world", "40", "2", "2", "0"};
    char v[] = "tb";
    CHECK(call_newt("TextboxReflowed",
                    "left top text width flexDown flexUp flags",
                    fake_textbox_reflowed, v, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("TextboxReflowed: too few args → FAILURE",
          "[new_wrappers][TextboxReflowed]") {
    WordListBuilder wl{"cmd", "0", "0", "hello world", "40", "2"};  // missing flexUp, flags
    CHECK(call_newt("TextboxReflowed",
                    "left top text width flexDown flexUp flags",
                    fake_textbox_reflowed, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── ReflowText — hand-written logic ──────────────────────────────────────────
// char* newtReflowText(char* text, int width, int fd, int fu, int* aw, int* ah)
// ═════════════════════════════════════════════════════════════════════════════

static char fake_reflow_buf[] = "reflowed text";
static char* fake_reflow_text(char*, int, int, int, int* aw, int* ah) {
    *aw = 13; *ah = 1;
    // Return a heap-allocated copy to mimic real newtReflowText
    char* p = reinterpret_cast<char*>(xmalloc(sizeof(fake_reflow_buf)));
    std::memcpy(p, fake_reflow_buf, sizeof(fake_reflow_buf));
    return p;
}

static int test_wrap_ReflowText(char* v, WORD_LIST* a) {
    char*       text;
    int         width, fd, fu;
    const char* wv;
    const char* hv;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, text))  goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, width)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, fd))    goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, fu))    goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, wv))    goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, hv))    goto usage;
    {
        int aw = 0, ah = 0;
        char* result = fake_reflow_text(text, width, fd, fu, &aw, &ah);
        if (v && result) {
            bind_variable(v, result, 0);
        }
        bind_variable(const_cast<char*>(wv),
                      const_cast<char*>(to_bash_string(aw).c_str()), 0);
        bind_variable(const_cast<char*>(hv),
                      const_cast<char*>(to_bash_string(ah).c_str()), 0);
        xfree(result);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("ReflowText: text width fd fu wv hv → SUCCESS, values bound",
          "[new_wrappers][ReflowText]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "hello world", "40", "2", "2", "W", "H"};
    char v[] = "out";
    REQUIRE(test_wrap_ReflowText(v, wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("out") != nullptr); CHECK(*last_bound("out") == "reflowed text");
    REQUIRE(last_bound("W")   != nullptr); CHECK(*last_bound("W")   == "13");
    REQUIRE(last_bound("H")   != nullptr); CHECK(*last_bound("H")   == "1");
}
TEST_CASE("ReflowText: too few args → FAILURE", "[new_wrappers][ReflowText]") {
    WordListBuilder wl{"cmd", "text", "40", "2", "2", "W"};  // missing H
    CHECK(test_wrap_ReflowText(nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── CreateGrid ────────────────────────────────────────────────────────────────
// newtGrid newtCreateGrid(int cols, int rows)
// ═════════════════════════════════════════════════════════════════════════════

static newtGrid fake_create_grid(int, int) { return nullptr; }

TEST_CASE("CreateGrid: cols rows → SUCCESS", "[new_wrappers][CreateGrid]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "3", "2"};
    char v[] = "g";
    CHECK(call_newt("CreateGrid", "cols rows",
                    fake_create_grid, v, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("CreateGrid: too few args → FAILURE", "[new_wrappers][CreateGrid]") {
    WordListBuilder wl{"cmd", "3"};   // missing rows
    CHECK(call_newt("CreateGrid", "cols rows",
                    fake_create_grid, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── GridSetField ──────────────────────────────────────────────────────────────
// void newtGridSetField(newtGrid, int col, int row, enum newtGridElement type,
//                       void* val, int padL, int padT, int padR, int padB,
//                       int anchor, int flags)
// ═════════════════════════════════════════════════════════════════════════════

static void fake_grid_set_field(newtGrid, int, int, newtGridElement, void*,
                                int, int, int, int, int, int) {}

TEST_CASE("GridSetField: 11 args → SUCCESS", "[new_wrappers][GridSetField]") {
    WordListBuilder wl{"cmd", GRID, "0", "0", "1", "",
                       "0", "0", "0", "0", "0", "0"};
    CHECK(call_newt("GridSetField",
                    "grid col row type val padLeft padTop padRight padBottom anchor flags",
                    fake_grid_set_field, nullptr, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("GridSetField: too few args → FAILURE", "[new_wrappers][GridSetField]") {
    WordListBuilder wl{"cmd", GRID, "0", "0", "1", "", "0"};  // only 6
    CHECK(call_newt("GridSetField",
                    "grid col row type val padLeft padTop padRight padBottom anchor flags",
                    fake_grid_set_field, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── GridGetSize — hand-written logic ─────────────────────────────────────────
// void newtGridGetSize(newtGrid grid, int* width, int* height)
// ═════════════════════════════════════════════════════════════════════════════

static void fake_grid_get_size(newtGrid, int* w, int* h) { *w = 50; *h = 20; }

static int test_wrap_GridGetSize(WORD_LIST* a) {
    newtGrid    grid;
    const char* wv;
    const char* hv;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, grid)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, wv))   goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, hv))   goto usage;
    {
        int w = 0, h = 0;
        fake_grid_get_size(grid, &w, &h);
        bind_variable(const_cast<char*>(wv),
                      const_cast<char*>(to_bash_string(w).c_str()), 0);
        bind_variable(const_cast<char*>(hv),
                      const_cast<char*>(to_bash_string(h).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("GridGetSize: grid widthVar heightVar → SUCCESS, values bound",
          "[new_wrappers][GridGetSize]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", GRID, "W", "H"};
    REQUIRE(test_wrap_GridGetSize(wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("W") != nullptr); CHECK(*last_bound("W") == "50");
    REQUIRE(last_bound("H") != nullptr); CHECK(*last_bound("H") == "20");
}
TEST_CASE("GridGetSize: too few args → FAILURE", "[new_wrappers][GridGetSize]") {
    WordListBuilder wl{"cmd", GRID, "W"};   // missing heightVar
    CHECK(test_wrap_GridGetSize(wl.head()) == EXECUTION_FAILURE);
}
TEST_CASE("GridGetSize: no args → FAILURE", "[new_wrappers][GridGetSize]") {
    WordListBuilder wl{"cmd"};
    CHECK(test_wrap_GridGetSize(wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── GridWrappedWindow ─────────────────────────────────────────────────────────
// void newtGridWrappedWindow(newtGrid, char* title)
// ═════════════════════════════════════════════════════════════════════════════

static void fake_grid_wrapped_window(newtGrid, char*) {}

TEST_CASE("GridWrappedWindow: grid title → SUCCESS",
          "[new_wrappers][GridWrappedWindow]") {
    WordListBuilder wl{"cmd", GRID, "My Title"};
    CHECK(call_newt("GridWrappedWindow", "grid title",
                    fake_grid_wrapped_window, nullptr, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("GridWrappedWindow: too few args → FAILURE",
          "[new_wrappers][GridWrappedWindow]") {
    WordListBuilder wl{"cmd", GRID};   // missing title
    CHECK(call_newt("GridWrappedWindow", "grid title",
                    fake_grid_wrapped_window, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── GridWrappedWindowAt ───────────────────────────────────────────────────────
// void newtGridWrappedWindowAt(newtGrid, char* title, int left, int top)
// ═════════════════════════════════════════════════════════════════════════════

static void fake_grid_wrapped_window_at(newtGrid, char*, int, int) {}

TEST_CASE("GridWrappedWindowAt: grid title left top → SUCCESS",
          "[new_wrappers][GridWrappedWindowAt]") {
    WordListBuilder wl{"cmd", GRID, "Title", "5", "3"};
    CHECK(call_newt("GridWrappedWindowAt", "grid title left top",
                    fake_grid_wrapped_window_at, nullptr, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("GridWrappedWindowAt: too few args → FAILURE",
          "[new_wrappers][GridWrappedWindowAt]") {
    WordListBuilder wl{"cmd", GRID, "Title", "5"};   // missing top
    CHECK(call_newt("GridWrappedWindowAt", "grid title left top",
                    fake_grid_wrapped_window_at, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── GridAddComponentsToForm ───────────────────────────────────────────────────
// void newtGridAddComponentsToForm(newtGrid, newtComponent form, int recurse)
// ═════════════════════════════════════════════════════════════════════════════

static void fake_grid_add_components(newtGrid, newtComponent, int) {}

TEST_CASE("GridAddComponentsToForm: grid form recurse → SUCCESS",
          "[new_wrappers][GridAddComponentsToForm]") {
    WordListBuilder wl{"cmd", GRID, CO, "1"};
    CHECK(call_newt("GridAddComponentsToForm", "grid form recurse",
                    fake_grid_add_components, nullptr, wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("GridAddComponentsToForm: too few args → FAILURE",
          "[new_wrappers][GridAddComponentsToForm]") {
    WordListBuilder wl{"cmd", GRID, CO};   // missing recurse
    CHECK(call_newt("GridAddComponentsToForm", "grid form recurse",
                    fake_grid_add_components, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── WinMessage — hand-written logic ──────────────────────────────────────────
// void newtWinMessage(char* title, char* btn, char* fmt, ...)
// ═════════════════════════════════════════════════════════════════════════════

static std::string last_win_message_text;
static void fake_win_message(char* /*title*/, char* /*btn*/,
                             char* fmt, const char* text) {
    last_win_message_text = text ? text : "";
    (void)fmt;
}

static int test_wrap_WinMessage(WORD_LIST* a) {
    const char* title;
    const char* btn;
    const char* text;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, title)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, btn))   goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, text))  goto usage;
    fake_win_message(const_cast<char*>(title), const_cast<char*>(btn),
                     const_cast<char*>("%s"), text);
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("WinMessage: title button text → SUCCESS",
          "[new_wrappers][WinMessage]") {
    last_win_message_text.clear();
    WordListBuilder wl{"cmd", "Alert", "OK", "Hello World"};
    REQUIRE(test_wrap_WinMessage(wl.head()) == EXECUTION_SUCCESS);
    CHECK(last_win_message_text == "Hello World");
}
TEST_CASE("WinMessage: too few args → FAILURE", "[new_wrappers][WinMessage]") {
    WordListBuilder wl{"cmd", "Alert", "OK"};   // missing text
    CHECK(test_wrap_WinMessage(wl.head()) == EXECUTION_FAILURE);
}
TEST_CASE("WinMessage: no args → FAILURE", "[new_wrappers][WinMessage]") {
    WordListBuilder wl{"cmd"};
    CHECK(test_wrap_WinMessage(wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── WinChoice — hand-written logic ───────────────────────────────────────────
// int newtWinChoice(char* title, char* b1, char* b2, char* fmt, ...)
// ═════════════════════════════════════════════════════════════════════════════

static int fake_win_choice(char*, char*, char*, char*, const char*) { return 1; }

static int test_wrap_WinChoice(char* v, WORD_LIST* a) {
    const char* title;
    const char* btn1;
    const char* btn2;
    const char* text;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, title)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, btn1))  goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, btn2))  goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, text))  goto usage;
    {
        int rc = fake_win_choice(const_cast<char*>(title),
                                 const_cast<char*>(btn1),
                                 const_cast<char*>(btn2),
                                 const_cast<char*>("%s"), text);
        if (v) {
            std::string s = to_bash_string(rc);
            bind_variable(v, const_cast<char*>(s.c_str()), 0);
        }
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("WinChoice: title b1 b2 text → SUCCESS, value bound",
          "[new_wrappers][WinChoice]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "Q", "Yes", "No", "Are you sure?"};
    char v[] = "rc";
    REQUIRE(test_wrap_WinChoice(v, wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("rc") != nullptr);
    CHECK(*last_bound("rc") == "1");
}
TEST_CASE("WinChoice: too few args → FAILURE", "[new_wrappers][WinChoice]") {
    WordListBuilder wl{"cmd", "Q", "Yes", "No"};   // missing text
    CHECK(test_wrap_WinChoice(nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── WinTernary — hand-written logic ──────────────────────────────────────────
// int newtWinTernary(char* title, char* b1, char* b2, char* b3, char* fmt, ...)
// ═════════════════════════════════════════════════════════════════════════════

static int fake_win_ternary(char*, char*, char*, char*, char*, const char*) { return 2; }

static int test_wrap_WinTernary(char* v, WORD_LIST* a) {
    const char* title;
    const char* btn1;
    const char* btn2;
    const char* btn3;
    const char* text;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, title)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, btn1))  goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, btn2))  goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, btn3))  goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, text))  goto usage;
    {
        int rc = fake_win_ternary(const_cast<char*>(title),
                                  const_cast<char*>(btn1),
                                  const_cast<char*>(btn2),
                                  const_cast<char*>(btn3),
                                  const_cast<char*>("%s"), text);
        if (v) {
            std::string s = to_bash_string(rc);
            bind_variable(v, const_cast<char*>(s.c_str()), 0);
        }
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("WinTernary: title b1 b2 b3 text → SUCCESS, value bound",
          "[new_wrappers][WinTernary]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "Q", "Yes", "No", "Cancel", "Really?"};
    char v[] = "rc";
    REQUIRE(test_wrap_WinTernary(v, wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("rc") != nullptr);
    CHECK(*last_bound("rc") == "2");
}
TEST_CASE("WinTernary: too few args → FAILURE", "[new_wrappers][WinTernary]") {
    WordListBuilder wl{"cmd", "Q", "Yes", "No", "Cancel"};   // missing text
    CHECK(test_wrap_WinTernary(nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── WinMenu — hand-written: parses fixed args + items + buttons ────────────────
// ═════════════════════════════════════════════════════════════════════════════

static int fake_win_menu_result = 1;
static int fake_win_menu(char*, char*, int, int, int, int,
                         char**, int* listitem, char*, void*) {
    *listitem = 2;
    return fake_win_menu_result;
}

static int test_wrap_WinMenu(char* v, WORD_LIST* a) {
    const char* title;
    const char* text;
    int         suggested_w, flex_down, flex_up, max_height;
    const char* listitem_var;
    int         num_items;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, title))       goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, text))        goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, suggested_w)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, flex_down))   goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, flex_up))     goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, max_height))  goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, listitem_var)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, num_items))   goto usage;
    {
        std::vector<const char*> items;
        for (int i = 0; i < num_items; ++i) {
            if (!a->next) goto usage;
            a = a->next;
            const char* s;
            if (!from_string(a->word->word, s)) goto usage;
            items.push_back(s);
        }
        items.push_back(nullptr);
        std::vector<std::string> btns;
        while (a->next) {
            a = a->next;
            btns.push_back(a->word->word);
        }
        if (btns.empty()) goto usage;
        int listitem = 0;
        int rc = fake_win_menu(const_cast<char*>(title),
                               const_cast<char*>(text),
                               suggested_w, flex_down, flex_up, max_height,
                               const_cast<char**>(items.data()), &listitem,
                               const_cast<char*>(btns[0].c_str()), nullptr);
        bind_variable(const_cast<char*>(listitem_var),
                      const_cast<char*>(to_bash_string(listitem).c_str()), 0);
        if (v) {
            std::string s = to_bash_string(rc);
            bind_variable(v, const_cast<char*>(s.c_str()), 0);
        }
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("WinMenu: full args → SUCCESS, listitem and rc bound",
          "[new_wrappers][WinMenu]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "Menu", "Pick one", "40", "3", "3", "6",
                       "sel", "2", "Alpha", "Beta",
                       "OK"};
    char v[] = "rc";
    REQUIRE(test_wrap_WinMenu(v, wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("rc")  != nullptr); CHECK(*last_bound("rc")  == "1");
    REQUIRE(last_bound("sel") != nullptr); CHECK(*last_bound("sel") == "2");
}
TEST_CASE("WinMenu: missing buttons → FAILURE", "[new_wrappers][WinMenu]") {
    WordListBuilder wl{"cmd", "Menu", "text", "40", "3", "3", "6",
                       "sel", "1", "Alpha"};   // no buttons
    CHECK(test_wrap_WinMenu(nullptr, wl.head()) == EXECUTION_FAILURE);
}
TEST_CASE("WinMenu: too few header args → FAILURE", "[new_wrappers][WinMenu]") {
    WordListBuilder wl{"cmd", "Menu"};   // missing most args
    CHECK(test_wrap_WinMenu(nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── ButtonBar — hand-written: pairs of label + compVar ───────────────────────
// ═════════════════════════════════════════════════════════════════════════════

static _newtGrid_tag      fake_button_bar_grid_obj;
static newtGrid           fake_button_bar_result = &fake_button_bar_grid_obj;
static _newtComponent_tag fake_btn_comp_objects[4];
static newtComponent      fake_btn_comps[4] = {
    &fake_btn_comp_objects[0], &fake_btn_comp_objects[1],
    &fake_btn_comp_objects[2], &fake_btn_comp_objects[3],
};

// Fake that fills first two comp slots and returns a non-null grid.
static newtGrid fake_button_bar(char*, newtComponent* c0,
                                char*, newtComponent* c1, void*) {
    *c0 = fake_btn_comps[0];
    *c1 = fake_btn_comps[1];
    return fake_button_bar_result;
}

static int test_wrap_ButtonBar(char* v, WORD_LIST* a) {
    struct BtnEntry { std::string label; std::string var; };
    std::vector<BtnEntry> entries;
    while (a->next) {
        a = a->next;
        BtnEntry e;
        e.label = a->word->word;
        if (!a->next) goto usage;
        a = a->next;
        e.var = a->word->word;
        entries.push_back(e);
    }
    if (entries.empty()) goto usage;
    {
        std::vector<newtComponent> comps(entries.size(), nullptr);
        // Only test with exactly 2 entries using the fake above.
        if (entries.size() == 2) {
            newtGrid g = fake_button_bar(
                const_cast<char*>(entries[0].label.c_str()), &comps[0],
                const_cast<char*>(entries[1].label.c_str()), &comps[1],
                nullptr);
            for (size_t i = 0; i < entries.size(); ++i) {
                std::string val = to_bash_string(comps[i]);
                bind_variable(const_cast<char*>(entries[i].var.c_str()),
                              const_cast<char*>(val.c_str()), 0);
            }
            if (v && g) {
                std::string s = to_bash_string(g);
                bind_variable(v, const_cast<char*>(s.c_str()), 0);
            }
        }
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("ButtonBar: label1 var1 label2 var2 → SUCCESS, comps bound",
          "[new_wrappers][ButtonBar]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "OK", "b1", "Cancel", "b2"};
    char v[] = "bg";
    REQUIRE(test_wrap_ButtonBar(v, wl.head()) == EXECUTION_SUCCESS);
    // Both comp variables should be bound to non-empty strings.
    REQUIRE(last_bound("b1") != nullptr);
    REQUIRE(last_bound("b2") != nullptr);
    CHECK(!last_bound("b1")->empty());
    CHECK(!last_bound("b2")->empty());
}
TEST_CASE("ButtonBar: no args → FAILURE", "[new_wrappers][ButtonBar]") {
    WordListBuilder wl{"cmd"};
    CHECK(test_wrap_ButtonBar(nullptr, wl.head()) == EXECUTION_FAILURE);
}
TEST_CASE("ButtonBar: odd args (label without var) → FAILURE",
          "[new_wrappers][ButtonBar]") {
    WordListBuilder wl{"cmd", "OK"};   // label without companion var
    CHECK(test_wrap_ButtonBar(nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ═════════════════════════════════════════════════════════════════════════════
// ── GridVStacked / GridHStacked / GridVCloseStacked / GridHCloseStacked ───────
// The four variadic stacked-grid wrappers share the same argument-parsing
// logic (grid_stacked_helper in newt_wrappers.cpp).  We re-implement that
// logic here against a fake function so the unit tests remain stub-only.
// ═════════════════════════════════════════════════════════════════════════════

static _newtGrid_tag fake_stacked_grid_obj;
static newtGrid      fake_stacked_result = &fake_stacked_grid_obj;

static newtGrid fake_stacked_fn(enum newtGridElement, void*, ...) {
    return fake_stacked_result;
}

// Mirrors grid_stacked_helper() in newt_wrappers.cpp using fake_stacked_fn.
static int test_wrap_GridStacked(char* v, WORD_LIST* a) {
    std::vector<std::pair<enum newtGridElement, void*>> pairs;
    while (a->next) {
        a = a->next;
        intmax_t t;
        if (!legal_number(a->word->word, &t)) goto usage;
        auto type = static_cast<enum newtGridElement>(t);
        if (!a->next) goto usage;   // type without val → failure
        a = a->next;
        void* val_p = nullptr;
        from_string(a->word->word, val_p);
        pairs.push_back({type, val_p});
    }
    if (pairs.empty()) goto usage;
    {
        newtGrid g = fake_stacked_fn(pairs[0].first, pairs[0].second,
                                     NEWT_GRID_EMPTY, nullptr);
        if (!g) goto usage;
        if (v) {
            std::string s = to_bash_string(g);
            bind_variable(v, const_cast<char*>(s.c_str()), 0);
        }
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("GridVStacked/HStacked: type1 val1 → SUCCESS, grid bound",
          "[new_wrappers][GridStacked]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "1", ""};   // NEWT_GRID_COMPONENT, nullptr component
    char v[] = "g";
    REQUIRE(test_wrap_GridStacked(v, wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("g") != nullptr);
    CHECK(!last_bound("g")->empty());
}

TEST_CASE("GridVStacked/HStacked: multiple pairs → SUCCESS",
          "[new_wrappers][GridStacked]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "1", "", "1", ""};
    char v[] = "g";
    REQUIRE(test_wrap_GridStacked(v, wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("g") != nullptr);
}

TEST_CASE("GridVStacked/HStacked: no args → FAILURE",
          "[new_wrappers][GridStacked]") {
    WordListBuilder wl{"cmd"};
    CHECK(test_wrap_GridStacked(nullptr, wl.head()) == EXECUTION_FAILURE);
}

TEST_CASE("GridVStacked/HStacked: type without val (odd args) → FAILURE",
          "[new_wrappers][GridStacked]") {
    WordListBuilder wl{"cmd", "1"};   // type without companion value
    CHECK(test_wrap_GridStacked(nullptr, wl.head()) == EXECUTION_FAILURE);
}
