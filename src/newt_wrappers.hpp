/**
 * newt_wrappers.hpp
 *
 * Declares the wrapper dispatch interface used by the bash builtin entry point.
 *
 * NOTE: The caller must include bash headers (for WORD_LIST) before this file.
 */

#pragma once

// Signature shared by every wrap_* function and expected by newt_builtin.
using WrapperFn = int(*)(char*, WORD_LIST*);

// Returns the wrapper for 'name', or nullptr if not found.
WrapperFn find_command(const char* name);
