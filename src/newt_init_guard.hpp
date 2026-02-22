#pragma once

/**
 * newt_init_guard.hpp
 *
 * Tracks whether newtInit() has been called and newtFinished() has not yet
 * been invoked.  Centralises the flag so it can be tested in isolation
 * (test_wrappers.cpp includes this header without linking against libnewt).
 *
 * Usage in newt_wrappers.cpp:
 *   newt_init_guard::set_initialized();    // after newtInit()
 *   newt_init_guard::clear_initialized();  // after newtFinished()
 *   newt_init_guard::is_initialized();     // guard in wrap_Finished
 *
 * Usage in unit tests:
 *   newt_init_guard::clear_initialized();  // reset to known state
 *   newt_init_guard::set_initialized();    // simulate a prior Init
 */

namespace newt_init_guard {

inline bool& flag() {
    static bool f = false;
    return f;
}

inline bool is_initialized()    { return flag(); }
inline void set_initialized()   { flag() = true; }
inline void clear_initialized() { flag() = false; }

} // namespace newt_init_guard
