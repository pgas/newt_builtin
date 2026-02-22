/*
 * newt.cpp — bash loadable builtin entry point for the libnewt wrapper.
 *
 * Load with:  enable -f ./newt.so newt
 * Usage:      newt [-v varname] SubCommand [args...]
 *
 * This file contains only the bash builtin boilerplate (newt_doc, newt_struct,
 * newt_builtin, load/unload callbacks).  The libnewt wrappers and dispatch
 * table live in newt_wrappers.cpp.
 */

#include <config.h>

#if defined(HAVE_UNISTD_H)
#  include <unistd.h>
#endif

#include <cstdio>

extern "C" {
#include "builtins.h"
#include "shell.h"
#include "bashgetopt.h"
#include "common.h"
}

#include "newt_wrappers.hpp"   // WrapperFn, find_command
#include "newt_constants.hpp"  // register_newt_constants

// ─── bash builtin boilerplate ─────────────────────────────────────────────────

// Forward-declare with C linkage so bash can look these up by symbol name.
extern "C" char*         newt_doc[];
extern "C" struct builtin newt_struct;
extern "C" int            newt_builtin(WORD_LIST*);
extern "C" int            newt_builtin_load(char*);
extern "C" void           newt_builtin_unload(char*);

char* newt_doc[] = {
    (char*)"Bash interface to libnewt.",
    (char*)"",
    (char*)"Usage: newt [-v varname] SubCommand [args...]",
    (char*)"",
    (char*)"SubCommand is a libnewt function name without the 'newt' prefix,",
    (char*)"e.g. 'newt OpenWindow 10 5 40 10 MyTile'.",
    (char*)"Use -v to capture the return value into a variable.",
    (char*)nullptr
};

struct builtin newt_struct = {
    (char*)"newt",
    newt_builtin,
    BUILTIN_ENABLED,
    newt_doc,
    (char*)"newt [-v varname] SubCommand [args...]",
    0
};

extern "C" int newt_builtin(WORD_LIST* list) {
    char* vname = nullptr;
    int c;

    reset_internal_getopt();
    while ((c = internal_getopt(list, const_cast<char*>("v:"))) != -1) {
        switch (c) {
        case 'v':
            vname = list_optarg;
#if defined(ARRAY_VARS)
            if (!legal_identifier(vname) && !valid_array_reference(vname, 0))
#else
            if (!legal_identifier(vname))
#endif
            {
                sh_invalidid(vname);
                return EX_USAGE;
            }
            break;
        case GETOPT_HELP:
            builtin_help();
            return EX_USAGE;
        default:
            builtin_usage();
            return EX_USAGE;
        }
    }

    list = loptend;
    if (list == nullptr) {
        builtin_usage();
        return EX_USAGE;
    }

    const char* subcmd = list->word->word;
    WrapperFn fn = find_command(subcmd);
    if (!fn) {
        std::fprintf(stderr, "newt: unknown subcommand '%s'\n", subcmd);
        return EXECUTION_FAILURE;
    }

    return fn(vname, list);
}

extern "C" int newt_builtin_load(char* /*s*/) {
    register_newt_constants();
    return 1;   // 1 = success for load callbacks
}

extern "C" void newt_builtin_unload(char* /*s*/) {
}
