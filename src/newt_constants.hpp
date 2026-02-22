/**
 * newt_constants.hpp
 *
 * Exports libnewt #define constants into bash as read-only associative arrays.
 *
 * Called once from newt_builtin_load() so the arrays are available as soon as
 * the builtin is loaded.
 *
 * Arrays created:
 *   NEWT_COLORSET   – colorset indices (ROOT, BORDER, WINDOW, …)
 *   NEWT_FLAG       – component flags (SCROLL, HIDDEN, DISABLED, …)
 *   NEWT_FD         – fd-watch flags (READ, WRITE, EXCEPT)
 *   NEWT_ARG        – special argument sentinels (LAST, APPEND)
 *   NEWT_CHECKBOXTREE – checkbox-tree flags and state chars
 *   NEWT_KEY        – key codes (TAB, ENTER, UP, DOWN, F1-F12, …)
 *   NEWT_ANCHOR     – grid anchor bits (LEFT, RIGHT, TOP, BOTTOM)
 *   NEWT_GRID_FLAG  – grid flags (GROWX, GROWY)
 *
 * Usage in bash:
 *   newt Init
 *   newt OpenWindow 2 2 40 10 "Test"
 *   newt Label 1 1 "hello"   # not using any constant here
 *   newt ListboxSetCurrent "$lb" "${NEWT_ARG[APPEND]}"
 *   newt CheckboxSetFlags   "$cb" "${NEWT_FLAG[SCROLL]}" 1
 */

#pragma once

/// Register all NEWT_* #define constants as read-only bash associative arrays.
/// Must be called only within a real bash environment (not in unit-test stubs).
void register_newt_constants();
