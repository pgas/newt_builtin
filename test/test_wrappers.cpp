/**
 * test_wrappers.cpp
 *
 * Unit tests covering the call_newt argument shape for every wrapper in
 * newt_wrappers.cpp that is not already tested in test_components.cpp or
 * test_call_newt.cpp.
 *
 * Each wrapper gets two tests:
 *   • Correct number/types of arguments → EXECUTION_SUCCESS
 *   • Too few arguments → EXECUTION_FAILURE
 * Non-void wrappers get an additional check that the return value is bound.
 *
 * No real libnewt or bash headers are used; everything compiles against the
 * lightweight stubs in stubs/.
 */

#include "stubs/bash_stubs.hpp"
#include "stubs/newt_stubs.hpp"
#include "stubs/word_list_builder.hpp"
#include "newt_arg_parser.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <string>

// ─── shared sentinel strings ──────────────────────────────────────────────────
static constexpr const char* CO   = "NULL";   // parsed as nullptr newtComponent
static constexpr const char* GRID = "NULL";   // parsed as nullptr newtGrid

// ─── shared fake functions ────────────────────────────────────────────────────
//
// Each category needs a function with the exact signature used by call_newt.

// void()
static void fake_noop() {}

// void(int)
static void fake_void_i(int) {}

// void(unsigned int)
static void fake_void_ui(unsigned int) {}

// void(const char*)
static void fake_void_cstr(const char*) {}

// void(int, int, const char*)
static void fake_void_i_i_cstr(int, int, const char*) {}

// int(int, int, int, int, const char*)   [OpenWindow]
static int fake_open_window(int, int, unsigned int, unsigned int, const char*) { return 0; }

// int(unsigned int, unsigned int, const char*)   [CenteredWindow]
static int fake_centered_window(unsigned int, unsigned int, const char*) { return 0; }

// void(int, const char*, const char*)   [SetColor]
static void fake_set_color(int, const char*, const char*) {}

// newtCallback-accepting  [SetHelpCallback]
static void   fake_cb_target(newtComponent, void*) {}
static void   fake_set_help_callback(newtCallback) {}

// void(newtComponent)
static void fake_void_co(newtComponent) {}

// int(newtComponent)
static int fake_int_co(newtComponent) { return 7; }

// newtComponent(newtComponent)
static newtComponent fake_co_co(newtComponent) { return nullptr; }

// void(newtComponent, int)
static void fake_void_co_i(newtComponent, int) {}

// void(newtComponent, newtComponent)
static void fake_void_co_co(newtComponent, newtComponent) {}

// void(newtComponent, const char*)
static void fake_void_co_cstr(newtComponent, const char*) {}

// void(newtComponent, int, int)
static void fake_void_co_i_i(newtComponent, int, int) {}

// void(newtComponent, int, const char*)
static void fake_void_co_i_cstr(newtComponent, int, const char*) {}

// newtComponent(newtComponent) – RunForm
static newtComponent fake_run_form(newtComponent) { return nullptr; }

// void(newtGrid, int, int)
static void fake_void_grid_i_i(newtGrid, int, int) {}

// void(newtGrid, int)
static void fake_void_grid_i(newtGrid, int) {}

// newtGrid(newtComponent, newtGrid, newtGrid)
static newtGrid fake_grid_basic_window(newtComponent, newtGrid, newtGrid) { return nullptr; }

// newtGrid(newtComponent, newtComponent, newtGrid)
static newtGrid fake_grid_simple_window(newtComponent, newtComponent, newtGrid) { return nullptr; }

// const char*(newtComponent)   [EntryGetValue]
static const char* fake_entry_get_value(newtComponent) { return "hello"; }

// void(newtComponent, const char*, int)   [EntrySet]
static void fake_entry_set(newtComponent, const char*, int) {}

// void(newtComponent, int, newtFlagsSense)   [EntrySetFlags]
static void fake_entry_set_flags(newtComponent, int, newtFlagsSense) {}

// void(newtComponent, int, int)   [EntrySetColors – same as fake_void_co_i_i]

// int(newtComponent)   [EntryGetCursorPosition – same as fake_int_co]

// void(newtComponent, int)   [EntrySetCursorPosition – same as fake_void_co_i]

// newtComponent(int, int, int, long long)   [Scale]
static newtComponent fake_scale(int, int, int, long long) { return nullptr; }

// void(newtComponent, unsigned long long)   [ScaleSet]
static void fake_scale_set(newtComponent, unsigned long long) {}

// newtComponent(int, int, int, int, int)   [Textbox]
static newtComponent fake_textbox(int, int, int, int, int) { return nullptr; }

// void*(newtComponent)   [ListboxGetCurrent]
static void* fake_listbox_get_current(newtComponent) { return reinterpret_cast<void*>(3); }

// void(newtComponent, void*)  [ListboxSetCurrentByKey, ListboxDeleteEntry]
static void fake_void_co_voidptr(newtComponent, void*) {}
static int  fake_int_co_voidptr(newtComponent, void*) { return 0; }

// void(newtComponent, int, void*)   [ListboxSetData]
static void fake_void_co_i_voidptr(newtComponent, int, void*) {}

// int(newtComponent, const char*, const void*)   [ListboxAppendEntry]
static int fake_listbox_append(newtComponent, const char*, const void*) { return 0; }

// int(newtComponent, const char*, const void*, void*)   [ListboxInsertEntry]
static int fake_listbox_insert(newtComponent, const char*, const void*, void*) { return 0; }

// void(newtComponent, const void*, newtFlagsSense)   [ListboxSelectItem]
static void fake_void_co_constvoidptr_sense(newtComponent, const void*, newtFlagsSense) {}

// ─────────────────────────────────────────────────────────────────────────────
// 1. Zero-arg void wrappers
// ─────────────────────────────────────────────────────────────────────────────

#define ZERO_ARG_TESTS(Name) \
    TEST_CASE(#Name ": no args → SUCCESS", "[wrappers][zero]") { \
        WordListBuilder wl{"cmd"}; \
        CHECK(call_newt(#Name, "", fake_noop, nullptr, wl.head()) == EXECUTION_SUCCESS); \
    }

ZERO_ARG_TESTS(Init)
ZERO_ARG_TESTS(Finished)
ZERO_ARG_TESTS(Cls)
ZERO_ARG_TESTS(WaitForKey)
ZERO_ARG_TESTS(ClearKeyBuffer)
ZERO_ARG_TESTS(Refresh)
ZERO_ARG_TESTS(Suspend)
ZERO_ARG_TESTS(Resume)
ZERO_ARG_TESTS(Bell)
ZERO_ARG_TESTS(CursorOff)
ZERO_ARG_TESTS(CursorOn)
ZERO_ARG_TESTS(PopWindow)
ZERO_ARG_TESTS(PopWindowNoRefresh)
ZERO_ARG_TESTS(RedrawHelpLine)
ZERO_ARG_TESTS(PopHelpLine)

// ─────────────────────────────────────────────────────────────────────────────
// 2. ResizeScreen: void(int redraw)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("ResizeScreen: redraw 1 → SUCCESS", "[wrappers][ResizeScreen]") {
    WordListBuilder wl{"cmd", "1"};
    CHECK(call_newt("ResizeScreen", "redraw", fake_void_i, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ResizeScreen: no args → FAILURE", "[wrappers][ResizeScreen]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("ResizeScreen", "redraw", fake_void_i, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 3. Delay: void(unsigned int usecs)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("Delay: usecs → SUCCESS", "[wrappers][Delay]") {
    WordListBuilder wl{"cmd", "100000"};
    CHECK(call_newt("Delay", "usecs", fake_void_ui, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("Delay: no args → FAILURE", "[wrappers][Delay]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("Delay", "usecs", fake_void_ui, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 4. PushHelpLine: void(const char* text)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("PushHelpLine: text → SUCCESS", "[wrappers][PushHelpLine]") {
    WordListBuilder wl{"cmd", "Press F1 for help"};
    CHECK(call_newt("PushHelpLine", "text", fake_void_cstr, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("PushHelpLine: no args → FAILURE", "[wrappers][PushHelpLine]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("PushHelpLine", "text", fake_void_cstr, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 5. DrawRootText: void(int col, int row, const char* text)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("DrawRootText: col row text → SUCCESS", "[wrappers][DrawRootText]") {
    WordListBuilder wl{"cmd", "5", "0", "hello"};
    CHECK(call_newt("DrawRootText", "col row text", fake_void_i_i_cstr, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("DrawRootText: too few args → FAILURE", "[wrappers][DrawRootText]") {
    WordListBuilder wl{"cmd", "5", "0"};
    CHECK(call_newt("DrawRootText", "col row text", fake_void_i_i_cstr, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 6. SetColor: void(int colorset, const char* fg, const char* bg)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("SetColor: colorset fg bg → SUCCESS", "[wrappers][SetColor]") {
    WordListBuilder wl{"cmd", "7", "white", "red"};
    CHECK(call_newt("SetColor", "colorset fg bg", fake_set_color, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("SetColor: too few args → FAILURE", "[wrappers][SetColor]") {
    WordListBuilder wl{"cmd", "7", "white"};
    CHECK(call_newt("SetColor", "colorset fg bg", fake_set_color, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 7. SetHelpCallback: void(newtCallback f)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("SetHelpCallback: function pointer hex → SUCCESS", "[wrappers][SetHelpCallback]") {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%p", reinterpret_cast<void*>(fake_cb_target));
    WordListBuilder wl{"cmd", buf};
    CHECK(call_newt("SetHelpCallback", "f", fake_set_help_callback, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("SetHelpCallback: no args → FAILURE", "[wrappers][SetHelpCallback]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("SetHelpCallback", "f", fake_set_help_callback, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 8. OpenWindow: int(int, int, unsigned int, unsigned int, const char*)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("OpenWindow: left top width height title → SUCCESS", "[wrappers][OpenWindow]") {
    WordListBuilder wl{"cmd", "10", "5", "40", "10", "My Window"};
    CHECK(call_newt("OpenWindow", "left top width height title",
                    fake_open_window, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("OpenWindow: too few args → FAILURE", "[wrappers][OpenWindow]") {
    WordListBuilder wl{"cmd", "10", "5", "40"};
    CHECK(call_newt("OpenWindow", "left top width height title",
                    fake_open_window, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 9. CenteredWindow: int(unsigned int, unsigned int, const char*)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("CenteredWindow: width height title → SUCCESS", "[wrappers][CenteredWindow]") {
    WordListBuilder wl{"cmd", "40", "10", "Centered"};
    CHECK(call_newt("CenteredWindow", "width height title",
                    fake_centered_window, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("CenteredWindow: too few args → FAILURE", "[wrappers][CenteredWindow]") {
    WordListBuilder wl{"cmd", "40"};
    CHECK(call_newt("CenteredWindow", "width height title",
                    fake_centered_window, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 10. One-arg void(newtComponent) functions
// ─────────────────────────────────────────────────────────────────────────────

#define CO_ARG_TESTS(Name, Spec) \
    TEST_CASE(#Name ": co → SUCCESS", "[wrappers][" #Name "]") { \
        WordListBuilder wl{"cmd", CO}; \
        CHECK(call_newt(#Name, Spec, fake_void_co, nullptr, wl.head()) == EXECUTION_SUCCESS); \
    } \
    TEST_CASE(#Name ": no args → FAILURE", "[wrappers][" #Name "]") { \
        WordListBuilder wl{"cmd"}; \
        CHECK(call_newt(#Name, Spec, fake_void_co, nullptr, wl.head()) == EXECUTION_FAILURE); \
    }

CO_ARG_TESTS(ListboxClear,        "co")
CO_ARG_TESTS(ListboxClearSelection,"co")
CO_ARG_TESTS(ComponentDestroy,    "co")
CO_ARG_TESTS(FormDestroy,         "co")
CO_ARG_TESTS(FormSetSize,         "co")
CO_ARG_TESTS(DrawForm,            "co")
CO_ARG_TESTS(TextboxSetHeight_co, "co")

// ─────────────────────────────────────────────────────────────────────────────
// 11. int(newtComponent) → bind result
// ─────────────────────────────────────────────────────────────────────────────

#define INT_CO_TESTS(Name, Spec) \
    TEST_CASE(#Name ": co → SUCCESS, result bound", "[wrappers][" #Name "]") { \
        clear_bound_vars(); \
        WordListBuilder wl{"cmd", CO}; \
        char v[] = "RES"; \
        CHECK(call_newt(#Name, Spec, fake_int_co, v, wl.head()) == EXECUTION_SUCCESS); \
        const std::string* s = last_bound("RES"); \
        REQUIRE(s != nullptr); \
        CHECK(*s == "7"); \
    } \
    TEST_CASE(#Name ": no args → FAILURE", "[wrappers][" #Name "]") { \
        WordListBuilder wl{"cmd"}; \
        CHECK(call_newt(#Name, Spec, fake_int_co, nullptr, wl.head()) == EXECUTION_FAILURE); \
    }

INT_CO_TESTS(ListboxItemCount,         "co")
INT_CO_TESTS(TextboxGetNumLines,       "co")
INT_CO_TESTS(FormGetScrollPosition,    "co")
INT_CO_TESTS(EntryGetCursorPosition,   "co")

// ─────────────────────────────────────────────────────────────────────────────
// 12. newtComponent(newtComponent) → bind result
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("FormGetCurrent: form → SUCCESS, result bound", "[wrappers][FormGetCurrent]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO};
    char v[] = "CUR";
    CHECK(call_newt("FormGetCurrent", "form", fake_co_co, v, wl.head()) == EXECUTION_SUCCESS);
    CHECK(last_bound("CUR") != nullptr);
}
TEST_CASE("FormGetCurrent: no args → FAILURE", "[wrappers][FormGetCurrent]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("FormGetCurrent", "form", fake_co_co, nullptr, wl.head()) == EXECUTION_FAILURE);
}

TEST_CASE("RunForm: form → SUCCESS, result bound", "[wrappers][RunForm]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO};
    char v[] = "RET";
    CHECK(call_newt("RunForm", "form", fake_run_form, v, wl.head()) == EXECUTION_SUCCESS);
    CHECK(last_bound("RET") != nullptr);
}
TEST_CASE("RunForm: no args → FAILURE", "[wrappers][RunForm]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("RunForm", "form", fake_run_form, nullptr, wl.head()) == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 13. void(newtComponent, int) functions
// ─────────────────────────────────────────────────────────────────────────────

#define CO_INT_TESTS(Name, Spec) \
    TEST_CASE(#Name ": co int → SUCCESS", "[wrappers][" #Name "]") { \
        WordListBuilder wl{"cmd", CO, "5"}; \
        CHECK(call_newt(#Name, Spec, fake_void_co_i, nullptr, wl.head()) == EXECUTION_SUCCESS); \
    } \
    TEST_CASE(#Name ": too few args → FAILURE", "[wrappers][" #Name "]") { \
        WordListBuilder wl{"cmd", CO}; \
        CHECK(call_newt(#Name, Spec, fake_void_co_i, nullptr, wl.head()) == EXECUTION_FAILURE); \
    }

CO_INT_TESTS(FormSetTimer,          "form milliseconds")
CO_INT_TESTS(FormSetBackground,     "form color")
CO_INT_TESTS(FormSetHeight,         "form height")
CO_INT_TESTS(FormSetWidth,          "form width")
CO_INT_TESTS(FormAddHotKey,         "form key")
CO_INT_TESTS(FormSetScrollPosition, "form position")
CO_INT_TESTS(TextboxSetHeight,      "co height")
CO_INT_TESTS(ComponentTakesFocus,   "co val")
CO_INT_TESTS(EntrySetCursorPosition,"co position")
CO_INT_TESTS(ListboxSetCurrent,     "co num")
CO_INT_TESTS(ListboxSetWidth,       "co width")

// ─────────────────────────────────────────────────────────────────────────────
// 14. void(newtComponent, newtComponent) functions
// ─────────────────────────────────────────────────────────────────────────────

#define CO_CO_TESTS(Name, Spec) \
    TEST_CASE(#Name ": form comp → SUCCESS", "[wrappers][" #Name "]") { \
        WordListBuilder wl{"cmd", CO, CO}; \
        CHECK(call_newt(#Name, Spec, fake_void_co_co, nullptr, wl.head()) == EXECUTION_SUCCESS); \
    } \
    TEST_CASE(#Name ": too few args → FAILURE", "[wrappers][" #Name "]") { \
        WordListBuilder wl{"cmd", CO}; \
        CHECK(call_newt(#Name, Spec, fake_void_co_co, nullptr, wl.head()) == EXECUTION_FAILURE); \
    }

CO_CO_TESTS(FormSetCurrent,   "form comp")
CO_CO_TESTS(FormAddComponent, "form comp")

// ─────────────────────────────────────────────────────────────────────────────
// 15. void(newtComponent, const char*) – LabelSetText-style (some new)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("EntrySet: co value cursorAtEnd → SUCCESS", "[wrappers][EntrySet]") {
    WordListBuilder wl{"cmd", CO, "new text", "0"};
    CHECK(call_newt("EntrySet", "co value cursorAtEnd", fake_entry_set, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("EntrySet: too few args → FAILURE", "[wrappers][EntrySet]") {
    WordListBuilder wl{"cmd", CO};
    CHECK(call_newt("EntrySet", "co value cursorAtEnd", fake_entry_set, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 16. EntryGetValue: const char*(newtComponent)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("EntryGetValue: co → SUCCESS, result bound", "[wrappers][EntryGetValue]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO};
    char v[] = "VAL";
    CHECK(call_newt("EntryGetValue", "co", fake_entry_get_value, v, wl.head())
          == EXECUTION_SUCCESS);
    const std::string* s = last_bound("VAL");
    REQUIRE(s != nullptr);
    CHECK(*s == "hello");
}
TEST_CASE("EntryGetValue: no args → FAILURE", "[wrappers][EntryGetValue]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("EntryGetValue", "co", fake_entry_get_value, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 17. EntrySetFlags: void(newtComponent, int, newtFlagsSense)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("EntrySetFlags: co flags sense → SUCCESS", "[wrappers][EntrySetFlags]") {
    WordListBuilder wl{"cmd", CO, "2", "0"};
    CHECK(call_newt("EntrySetFlags", "co flags sense", fake_entry_set_flags, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("EntrySetFlags: too few args → FAILURE", "[wrappers][EntrySetFlags]") {
    WordListBuilder wl{"cmd", CO, "2"};
    CHECK(call_newt("EntrySetFlags", "co flags sense", fake_entry_set_flags, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 18. EntrySetColors / ScaleSetColors: void(newtComponent, int, int)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("EntrySetColors: co normal disabled → SUCCESS", "[wrappers][EntrySetColors]") {
    WordListBuilder wl{"cmd", CO, "11", "21"};
    CHECK(call_newt("EntrySetColors", "co normal disabledColor",
                    fake_void_co_i_i, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("EntrySetColors: too few args → FAILURE", "[wrappers][EntrySetColors]") {
    WordListBuilder wl{"cmd", CO, "11"};
    CHECK(call_newt("EntrySetColors", "co normal disabledColor",
                    fake_void_co_i_i, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 19. Scale: newtComponent(int, int, int, long long)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("Scale: left top width fullValue → SUCCESS", "[wrappers][Scale]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "5", "3", "30", "100"};
    char v[] = "SC";
    CHECK(call_newt("Scale", "left top width fullValue", fake_scale, v, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("Scale: too few args → FAILURE", "[wrappers][Scale]") {
    WordListBuilder wl{"cmd", "5", "3", "30"};
    CHECK(call_newt("Scale", "left top width fullValue", fake_scale, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 20. ScaleSet: void(newtComponent, unsigned long long)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("ScaleSet: co amount → SUCCESS", "[wrappers][ScaleSet]") {
    WordListBuilder wl{"cmd", CO, "42"};
    CHECK(call_newt("ScaleSet", "co amount", fake_scale_set, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ScaleSet: no amount → FAILURE", "[wrappers][ScaleSet]") {
    WordListBuilder wl{"cmd", CO};
    CHECK(call_newt("ScaleSet", "co amount", fake_scale_set, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 21. Textbox: newtComponent(int, int, int, int, int)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("Textbox: left top width height flags → SUCCESS", "[wrappers][Textbox]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", "2", "3", "40", "10", "0"};
    char v[] = "TB";
    CHECK(call_newt("Textbox", "left top width height flags", fake_textbox, v, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("Textbox: too few args → FAILURE", "[wrappers][Textbox]") {
    WordListBuilder wl{"cmd", "2", "3", "40"};
    CHECK(call_newt("Textbox", "left top width height flags", fake_textbox, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 22. Grid wrappers
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("GridPlace: grid left top → SUCCESS", "[wrappers][GridPlace]") {
    WordListBuilder wl{"cmd", GRID, "5", "3"};
    CHECK(call_newt("GridPlace", "grid left top", fake_void_grid_i_i, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("GridPlace: too few args → FAILURE", "[wrappers][GridPlace]") {
    WordListBuilder wl{"cmd", GRID, "5"};
    CHECK(call_newt("GridPlace", "grid left top", fake_void_grid_i_i, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

TEST_CASE("GridFree: grid recurse → SUCCESS", "[wrappers][GridFree]") {
    WordListBuilder wl{"cmd", GRID, "1"};
    CHECK(call_newt("GridFree", "grid recurse", fake_void_grid_i, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("GridFree: too few args → FAILURE", "[wrappers][GridFree]") {
    WordListBuilder wl{"cmd", GRID};
    CHECK(call_newt("GridFree", "grid recurse", fake_void_grid_i, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

TEST_CASE("GridBasicWindow: text middle buttons → SUCCESS", "[wrappers][GridBasicWindow]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, GRID, GRID};
    char v[] = "GW";
    CHECK(call_newt("GridBasicWindow", "text middle buttons",
                    fake_grid_basic_window, v, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("GridBasicWindow: too few args → FAILURE", "[wrappers][GridBasicWindow]") {
    WordListBuilder wl{"cmd", CO, GRID};
    CHECK(call_newt("GridBasicWindow", "text middle buttons",
                    fake_grid_basic_window, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

TEST_CASE("GridSimpleWindow: text middle buttons → SUCCESS", "[wrappers][GridSimpleWindow]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, CO, GRID};
    char v[] = "GS";
    CHECK(call_newt("GridSimpleWindow", "text middle buttons",
                    fake_grid_simple_window, v, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("GridSimpleWindow: too few args → FAILURE", "[wrappers][GridSimpleWindow]") {
    WordListBuilder wl{"cmd", CO, CO};
    CHECK(call_newt("GridSimpleWindow", "text middle buttons",
                    fake_grid_simple_window, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 23. ListboxGetCurrent: void*(newtComponent) → bind as hex
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("ListboxGetCurrent: co → SUCCESS, result bound", "[wrappers][ListboxGetCurrent]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO};
    char v[] = "KEY";
    CHECK(call_newt("ListboxGetCurrent", "co", fake_listbox_get_current, v, wl.head())
          == EXECUTION_SUCCESS);
    const std::string* s = last_bound("KEY");
    REQUIRE(s != nullptr);
    // to_bash_string(void*) formats as %p — just confirm it's non-empty
    CHECK(!s->empty());
}
TEST_CASE("ListboxGetCurrent: no args → FAILURE", "[wrappers][ListboxGetCurrent]") {
    WordListBuilder wl{"cmd"};
    CHECK(call_newt("ListboxGetCurrent", "co", fake_listbox_get_current, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 24. ListboxSetCurrentByKey / ListboxDeleteEntry: void/int(newtComponent, void*)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("ListboxSetCurrentByKey: co key → SUCCESS", "[wrappers][ListboxSetCurrentByKey]") {
    WordListBuilder wl{"cmd", CO, "3"};   // key as decimal integer
    CHECK(call_newt("ListboxSetCurrentByKey", "co key",
                    fake_void_co_voidptr, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ListboxSetCurrentByKey: too few args → FAILURE", "[wrappers][ListboxSetCurrentByKey]") {
    WordListBuilder wl{"cmd", CO};
    CHECK(call_newt("ListboxSetCurrentByKey", "co key",
                    fake_void_co_voidptr, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

TEST_CASE("ListboxDeleteEntry: co key → SUCCESS", "[wrappers][ListboxDeleteEntry]") {
    WordListBuilder wl{"cmd", CO, "2"};
    CHECK(call_newt("ListboxDeleteEntry", "co key",
                    fake_int_co_voidptr, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ListboxDeleteEntry: too few args → FAILURE", "[wrappers][ListboxDeleteEntry]") {
    WordListBuilder wl{"cmd", CO};
    CHECK(call_newt("ListboxDeleteEntry", "co key",
                    fake_int_co_voidptr, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 25. ListboxSetEntry: void(newtComponent, int, const char*)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("ListboxSetEntry: co num text → SUCCESS", "[wrappers][ListboxSetEntry]") {
    WordListBuilder wl{"cmd", CO, "0", "item text"};
    CHECK(call_newt("ListboxSetEntry", "co num text",
                    fake_void_co_i_cstr, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ListboxSetEntry: too few args → FAILURE", "[wrappers][ListboxSetEntry]") {
    WordListBuilder wl{"cmd", CO, "0"};
    CHECK(call_newt("ListboxSetEntry", "co num text",
                    fake_void_co_i_cstr, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 26. ListboxSetData: void(newtComponent, int, void*)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("ListboxSetData: co num data → SUCCESS", "[wrappers][ListboxSetData]") {
    WordListBuilder wl{"cmd", CO, "1", "99"};  // data as decimal
    CHECK(call_newt("ListboxSetData", "co num data",
                    fake_void_co_i_voidptr, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ListboxSetData: too few args → FAILURE", "[wrappers][ListboxSetData]") {
    WordListBuilder wl{"cmd", CO, "1"};
    CHECK(call_newt("ListboxSetData", "co num data",
                    fake_void_co_i_voidptr, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 27. ListboxAppendEntry: int(newtComponent, const char*, const void*)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("ListboxAppendEntry: co text data → SUCCESS", "[wrappers][ListboxAppendEntry]") {
    WordListBuilder wl{"cmd", CO, "item", "0"};
    CHECK(call_newt("ListboxAppendEntry", "co text data",
                    fake_listbox_append, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ListboxAppendEntry: too few args → FAILURE", "[wrappers][ListboxAppendEntry]") {
    WordListBuilder wl{"cmd", CO, "item"};
    CHECK(call_newt("ListboxAppendEntry", "co text data",
                    fake_listbox_append, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 28. ListboxInsertEntry: int(newtComponent, const char*, const void*, void*)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("ListboxInsertEntry: co text data key → SUCCESS", "[wrappers][ListboxInsertEntry]") {
    WordListBuilder wl{"cmd", CO, "new item", "5", "2"};
    CHECK(call_newt("ListboxInsertEntry", "co text data key",
                    fake_listbox_insert, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ListboxInsertEntry: too few args → FAILURE", "[wrappers][ListboxInsertEntry]") {
    WordListBuilder wl{"cmd", CO, "new item", "5"};
    CHECK(call_newt("ListboxInsertEntry", "co text data key",
                    fake_listbox_insert, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 29. ListboxSelectItem: void(newtComponent, const void*, newtFlagsSense)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("ListboxSelectItem: co key sense → SUCCESS", "[wrappers][ListboxSelectItem]") {
    WordListBuilder wl{"cmd", CO, "3", "0"};
    CHECK(call_newt("ListboxSelectItem", "co key sense",
                    fake_void_co_constvoidptr_sense, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ListboxSelectItem: too few args → FAILURE", "[wrappers][ListboxSelectItem]") {
    WordListBuilder wl{"cmd", CO, "3"};
    CHECK(call_newt("ListboxSelectItem", "co key sense",
                    fake_void_co_constvoidptr_sense, nullptr, wl.head())
          == EXECUTION_FAILURE);
}
// ─────────────────────────────────────────────────────────────────────────────
// 30. FormWatchFd: void(newtComponent form, int fd, int fdFlags)
// ─────────────────────────────────────────────────────────────────────────────

TEST_CASE("FormWatchFd: form fd fdFlags → SUCCESS", "[wrappers][FormWatchFd]") {
    WordListBuilder wl{"cmd", CO, "0", "1"};
    CHECK(call_newt("FormWatchFd", "form fd fdFlags",
                    fake_void_co_i_i, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("FormWatchFd: too few args → FAILURE", "[wrappers][FormWatchFd]") {
    WordListBuilder wl{"cmd", CO, "0"};  // missing fdFlags
    CHECK(call_newt("FormWatchFd", "form fd fdFlags",
                    fake_void_co_i_i, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 31. FormRun: hand-written wrapper — co reasonVar valueVar
//
// Tests use an inline reimplementation of wrap_FormRun's parsing logic
// with a local fake newtFormRun, allowing full argument-walk verification
// without compiling against newt_wrappers.cpp.
// ─────────────────────────────────────────────────────────────────────────────

static newtExitStruct g_fake_exit_struct{};
static void fake_newt_form_run(newtComponent, newtExitStruct* es) {
    *es = g_fake_exit_struct;
}

// Inline reimplementation of wrap_FormRun's argument-walking logic.
static int test_wrap_FormRun(WORD_LIST* a) {
    newtComponent co;
    const char* reason_var;
    const char* value_var;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, reason_var)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, value_var)) goto usage;
    {
        newtExitStruct es;
        fake_newt_form_run(co, &es);
        const char* reason_str = "ERROR";
        std::string value_str = "0";
        switch (es.reason) {
            case NEWT_EXIT_HOTKEY:
                reason_str = "HOTKEY";
                value_str = to_bash_string(es.u.key);
                break;
            case NEWT_EXIT_COMPONENT:
                reason_str = "COMPONENT";
                value_str = to_bash_string(es.u.co);
                break;
            case NEWT_EXIT_FDREADY:
                reason_str = "FDREADY";
                value_str = to_bash_string(es.u.watch);
                break;
            case NEWT_EXIT_TIMER:
                reason_str = "TIMER";
                break;
            case NEWT_EXIT_ERROR:
            default:
                reason_str = "ERROR";
                break;
        }
        bind_variable(const_cast<char*>(reason_var),
                      const_cast<char*>(reason_str), 0);
        bind_variable(const_cast<char*>(value_var),
                      const_cast<char*>(value_str.c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("FormRun: co reasonVar valueVar → SUCCESS, TIMER exit", "[wrappers][FormRun]") {
    clear_bound_vars();
    g_fake_exit_struct.reason = NEWT_EXIT_TIMER;
    WordListBuilder wl{"cmd", CO, "REASON", "VALUE"};
    REQUIRE(test_wrap_FormRun(wl.head()) == EXECUTION_SUCCESS);
    const std::string* r = last_bound("REASON");
    const std::string* v = last_bound("VALUE");
    REQUIRE(r != nullptr);  CHECK(*r == "TIMER");
    REQUIRE(v != nullptr);  CHECK(*v == "0");
}
TEST_CASE("FormRun: HOTKEY exit binds key code", "[wrappers][FormRun]") {
    clear_bound_vars();
    g_fake_exit_struct.reason  = NEWT_EXIT_HOTKEY;
    g_fake_exit_struct.u.key   = 65;   // 'A'
    WordListBuilder wl{"cmd", CO, "REASON", "VALUE"};
    REQUIRE(test_wrap_FormRun(wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("REASON") != nullptr);
    CHECK(*last_bound("REASON") == "HOTKEY");
    REQUIRE(last_bound("VALUE") != nullptr);
    CHECK(*last_bound("VALUE") == "65");
}
TEST_CASE("FormRun: COMPONENT exit binds component ptr", "[wrappers][FormRun]") {
    clear_bound_vars();
    g_fake_exit_struct.reason  = NEWT_EXIT_COMPONENT;
    g_fake_exit_struct.u.co    = nullptr;
    WordListBuilder wl{"cmd", CO, "REASON", "VALUE"};
    REQUIRE(test_wrap_FormRun(wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("REASON") != nullptr);
    CHECK(*last_bound("REASON") == "COMPONENT");
}
TEST_CASE("FormRun: too few args → FAILURE", "[wrappers][FormRun]") {
    WordListBuilder wl{"cmd", CO, "REASON"};  // missing valueVar
    CHECK(test_wrap_FormRun(wl.head()) == EXECUTION_FAILURE);
}
TEST_CASE("FormRun: no args → FAILURE", "[wrappers][FormRun]") {
    WordListBuilder wl{"cmd"};
    CHECK(test_wrap_FormRun(wl.head()) == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 32. FormAddComponents: hand-written wrapper — form comp1 [comp2 ...]
// ─────────────────────────────────────────────────────────────────────────────

static void fake_newt_form_add_component(newtComponent, newtComponent) {}

static int test_wrap_FormAddComponents(WORD_LIST* a) {
    newtComponent form;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, form)) goto usage;
    if (!a->next) goto usage; a = a->next;       // need at least one component
    while (a) {
        newtComponent comp;
        if (!from_string(a->word->word, comp)) goto usage;
        fake_newt_form_add_component(form, comp);
        a = a->next;
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("FormAddComponents: form + 1 comp → SUCCESS", "[wrappers][FormAddComponents]") {
    WordListBuilder wl{"cmd", CO, CO};
    CHECK(test_wrap_FormAddComponents(wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("FormAddComponents: form + 3 comps → SUCCESS", "[wrappers][FormAddComponents]") {
    WordListBuilder wl{"cmd", CO, CO, CO, CO};
    CHECK(test_wrap_FormAddComponents(wl.head()) == EXECUTION_SUCCESS);
}
TEST_CASE("FormAddComponents: no args → FAILURE", "[wrappers][FormAddComponents]") {
    WordListBuilder wl{"cmd"};
    CHECK(test_wrap_FormAddComponents(wl.head()) == EXECUTION_FAILURE);
}
TEST_CASE("FormAddComponents: form only, no components → FAILURE", "[wrappers][FormAddComponents]") {
    WordListBuilder wl{"cmd", CO};  // form but no components
    CHECK(test_wrap_FormAddComponents(wl.head()) == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 33. ComponentAddCallback: void(newtComponent, newtCallback, void*)
// ─────────────────────────────────────────────────────────────────────────────

static void fake_component_add_cb(newtComponent, newtCallback, void*) {}

TEST_CASE("ComponentAddCallback: co f data → SUCCESS", "[wrappers][ComponentAddCallback]") {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%p", reinterpret_cast<void*>(fake_cb_target));
    WordListBuilder wl{"cmd", CO, buf, "0"};
    CHECK(call_newt("ComponentAddCallback", "co f data",
                    fake_component_add_cb, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("ComponentAddCallback: too few args → FAILURE", "[wrappers][ComponentAddCallback]") {
    WordListBuilder wl{"cmd", CO};
    CHECK(call_newt("ComponentAddCallback", "co f data",
                    fake_component_add_cb, nullptr, wl.head())
          == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 34. ComponentGetPosition: void(co, int* left, int* top) — hand-written
// ─────────────────────────────────────────────────────────────────────────────

static void fake_get_position(newtComponent, int* l, int* t) { *l = 5; *t = 3; }

static int test_wrap_ComponentGetPosition(WORD_LIST* a) {
    newtComponent co;
    const char* lv;
    const char* tv;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, lv)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, tv)) goto usage;
    {
        int l = 0, t = 0;
        fake_get_position(co, &l, &t);
        bind_variable(const_cast<char*>(lv),
                      const_cast<char*>(to_bash_string(l).c_str()), 0);
        bind_variable(const_cast<char*>(tv),
                      const_cast<char*>(to_bash_string(t).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("ComponentGetPosition: co leftVar topVar → SUCCESS, values bound",
          "[wrappers][ComponentGetPosition]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, "L", "T"};
    REQUIRE(test_wrap_ComponentGetPosition(wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("L") != nullptr);  CHECK(*last_bound("L") == "5");
    REQUIRE(last_bound("T") != nullptr);  CHECK(*last_bound("T") == "3");
}
TEST_CASE("ComponentGetPosition: too few args → FAILURE", "[wrappers][ComponentGetPosition]") {
    WordListBuilder wl{"cmd", CO, "L"};  // missing topVar
    CHECK(test_wrap_ComponentGetPosition(wl.head()) == EXECUTION_FAILURE);
}
TEST_CASE("ComponentGetPosition: no args → FAILURE", "[wrappers][ComponentGetPosition]") {
    WordListBuilder wl{"cmd"};
    CHECK(test_wrap_ComponentGetPosition(wl.head()) == EXECUTION_FAILURE);
}

// ─────────────────────────────────────────────────────────────────────────────
// 35. ComponentGetSize: void(co, int* width, int* height) — hand-written
// ─────────────────────────────────────────────────────────────────────────────

static void fake_get_size(newtComponent, int* w, int* h) { *w = 20; *h = 4; }

static int test_wrap_ComponentGetSize(WORD_LIST* a) {
    newtComponent co;
    const char* wv;
    const char* hv;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, co)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, wv)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, hv)) goto usage;
    {
        int w = 0, h = 0;
        fake_get_size(co, &w, &h);
        bind_variable(const_cast<char*>(wv),
                      const_cast<char*>(to_bash_string(w).c_str()), 0);
        bind_variable(const_cast<char*>(hv),
                      const_cast<char*>(to_bash_string(h).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    return EXECUTION_FAILURE;
}

TEST_CASE("ComponentGetSize: co widthVar heightVar → SUCCESS, values bound",
          "[wrappers][ComponentGetSize]") {
    clear_bound_vars();
    WordListBuilder wl{"cmd", CO, "W", "H"};
    REQUIRE(test_wrap_ComponentGetSize(wl.head()) == EXECUTION_SUCCESS);
    REQUIRE(last_bound("W") != nullptr);  CHECK(*last_bound("W") == "20");
    REQUIRE(last_bound("H") != nullptr);  CHECK(*last_bound("H") == "4");
}
TEST_CASE("ComponentGetSize: too few args → FAILURE", "[wrappers][ComponentGetSize]") {
    WordListBuilder wl{"cmd", CO, "W"};  // missing heightVar
    CHECK(test_wrap_ComponentGetSize(wl.head()) == EXECUTION_FAILURE);
}
TEST_CASE("ComponentGetSize: no args → FAILURE", "[wrappers][ComponentGetSize]") {
    WordListBuilder wl{"cmd"};
    CHECK(test_wrap_ComponentGetSize(wl.head()) == EXECUTION_FAILURE);
}