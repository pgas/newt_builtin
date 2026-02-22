/**
 * newt_stubs.hpp
 *
 * Minimal stubs for libnewt types used by newt_arg_parser.hpp, so we can
 * compile and test that header without linking against libnewt.
 */
#pragma once

#include <vector>

// ── opaque handle types ───────────────────────────────────────────────────────

struct _newtComponent_tag {};
using newtComponent = _newtComponent_tag*;

struct _newtGrid_tag {};
using newtGrid = _newtGrid_tag*;

// ── callback typedefs ─────────────────────────────────────────────────────────

using newtCallback        = void (*)(newtComponent, void*);
using newtSuspendCallback = void (*)(void*);
using newtEntryFilter     = char (*)(newtComponent, void*, int, int);

// ── enums ─────────────────────────────────────────────────────────────────────

enum newtFlagsSense {
    NEWT_FLAGS_SET    = 0,
    NEWT_FLAGS_RESET  = 1,
    NEWT_FLAGS_TOGGLE = 2,
};

enum newtGridElement {
    NEWT_GRID_COMPONENT = 0,
    NEWT_GRID_SUBGRID   = 1,
};

// ── newtComponentAddCallback stub ────────────────────────────────────────────

struct ComponentCallbackReg {
    newtComponent co;
    newtCallback  f;
    void*         data;
};
inline std::vector<ComponentCallbackReg>& component_callback_regs() {
    static std::vector<ComponentCallbackReg> v;
    return v;
}
inline void clear_component_callback_regs() { component_callback_regs().clear(); }
inline void newtComponentAddCallback(newtComponent co, newtCallback f, void* data) {
    component_callback_regs().push_back({co, f, data});
}

// ── newtExitStruct for FormRun ────────────────────────────────────────────────

enum newtExitReason {
    NEWT_EXIT_HOTKEY    = 0,
    NEWT_EXIT_COMPONENT = 1,
    NEWT_EXIT_FDREADY   = 2,
    NEWT_EXIT_TIMER     = 3,
    NEWT_EXIT_ERROR     = 4,
};

struct newtExitStruct {
    newtExitReason reason;
    union {
        int watch;
        int key;
        newtComponent co;
    } u;
};
