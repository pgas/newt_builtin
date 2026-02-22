/*
 * newt_constants.cpp — register NEWT_* #define constants as read-only bash
 * associative arrays.
 *
 * See newt_constants.hpp for the full list of arrays and usage examples.
 */

#include <config.h>

#if defined(HAVE_UNISTD_H)
#  include <unistd.h>
#endif

extern "C" {
#include <newt.h>
#include "builtins.h"
#include "shell.h"   // pulls in variables.h → assoc.h
}

#include <string>
#include <utility>
#include <initializer_list>

#include "newt_constants.hpp"

// ─── helpers ──────────────────────────────────────────────────────────────────

/// Create (or overwrite) an associative array variable called @p name,
/// populate it with @p entries, then mark it read-only.
static void make_readonly_assoc(
        const char* name,
        std::initializer_list<std::pair<const char*, int>> entries)
{
    SHELL_VAR* var = make_new_assoc_variable(const_cast<char*>(name));
    if (!var) return;

    HASH_TABLE* h = assoc_cell(var);
    for (auto& [key, val] : entries) {
        std::string s = std::to_string(val);
        assoc_insert(h,
                     const_cast<char*>(key),
                     const_cast<char*>(s.c_str()));
    }

    VSETATTR(var, att_readonly);
}

// ─── public entry point ───────────────────────────────────────────────────────

void register_newt_constants() {

    // ── NEWT_COLORSET ─────────────────────────────────────────────────────────
    // Usage: newt SetColor "${NEWT_COLORSET[BUTTON]}" fg bg
    make_readonly_assoc("NEWT_COLORSET", {
        { "ROOT",          NEWT_COLORSET_ROOT          },
        { "BORDER",        NEWT_COLORSET_BORDER        },
        { "WINDOW",        NEWT_COLORSET_WINDOW        },
        { "SHADOW",        NEWT_COLORSET_SHADOW        },
        { "TITLE",         NEWT_COLORSET_TITLE         },
        { "BUTTON",        NEWT_COLORSET_BUTTON        },
        { "ACTBUTTON",     NEWT_COLORSET_ACTBUTTON     },
        { "CHECKBOX",      NEWT_COLORSET_CHECKBOX      },
        { "ACTCHECKBOX",   NEWT_COLORSET_ACTCHECKBOX   },
        { "ENTRY",         NEWT_COLORSET_ENTRY         },
        { "LABEL",         NEWT_COLORSET_LABEL         },
        { "LISTBOX",       NEWT_COLORSET_LISTBOX       },
        { "ACTLISTBOX",    NEWT_COLORSET_ACTLISTBOX    },
        { "TEXTBOX",       NEWT_COLORSET_TEXTBOX       },
        { "ACTTEXTBOX",    NEWT_COLORSET_ACTTEXTBOX    },
        { "HELPLINE",      NEWT_COLORSET_HELPLINE      },
        { "ROOTTEXT",      NEWT_COLORSET_ROOTTEXT      },
        { "EMPTYSCALE",    NEWT_COLORSET_EMPTYSCALE    },
        { "FULLSCALE",     NEWT_COLORSET_FULLSCALE     },
        { "DISENTRY",      NEWT_COLORSET_DISENTRY      },
        { "COMPACTBUTTON", NEWT_COLORSET_COMPACTBUTTON },
        { "ACTSELLISTBOX", NEWT_COLORSET_ACTSELLISTBOX },
        { "SELLISTBOX",    NEWT_COLORSET_SELLISTBOX    },
    });

    // ── NEWT_FLAG ─────────────────────────────────────────────────────────────
    // Usage: newt CheckboxSetFlags "$cb" "${NEWT_FLAG[SCROLL]}" 1
    make_readonly_assoc("NEWT_FLAG", {
        { "RETURNEXIT", NEWT_FLAG_RETURNEXIT },
        { "HIDDEN",     NEWT_FLAG_HIDDEN     },
        { "SCROLL",     NEWT_FLAG_SCROLL     },
        { "DISABLED",   NEWT_FLAG_DISABLED   },
        { "BORDER",     NEWT_FLAG_BORDER     },
        { "WRAP",       NEWT_FLAG_WRAP       },
        { "NOF12",      NEWT_FLAG_NOF12      },
        { "MULTIPLE",   NEWT_FLAG_MULTIPLE   },
        { "SELECTED",   NEWT_FLAG_SELECTED   },
        { "CHECKBOX",   NEWT_FLAG_CHECKBOX   },
        { "PASSWORD",   NEWT_FLAG_PASSWORD   },
        { "SHOWCURSOR", NEWT_FLAG_SHOWCURSOR },
    });

    // ── NEWT_FD ───────────────────────────────────────────────────────────────
    // Usage: newt FormWatchFd "$form" "$fd" "${NEWT_FD[READ]}"
    make_readonly_assoc("NEWT_FD", {
        { "READ",   NEWT_FD_READ   },
        { "WRITE",  NEWT_FD_WRITE  },
        { "EXCEPT", NEWT_FD_EXCEPT },
    });

    // ── NEWT_ARG ──────────────────────────────────────────────────────────────
    // Usage: newt ListboxInsertEntry "$lb" "item" "$data" "${NEWT_ARG[APPEND]}"
    make_readonly_assoc("NEWT_ARG", {
        { "LAST",   NEWT_ARG_LAST   },
        { "APPEND", NEWT_ARG_APPEND },
    });

    // ── NEWT_CHECKBOXTREE ─────────────────────────────────────────────────────
    // Usage: newt CheckboxTreeAddItem "$t" "text" "$key" "${NEWT_CHECKBOXTREE[UNSELECTABLE]}" 0 "${NEWT_ARG[LAST]}"
    // State chars stored as their ASCII integer values:
    //   COLLAPSED=0, EXPANDED=1, UNSELECTED=32(' '), SELECTED=42('*')
    make_readonly_assoc("NEWT_CHECKBOXTREE", {
        { "UNSELECTABLE", NEWT_CHECKBOXTREE_UNSELECTABLE },
        { "HIDE_BOX",     NEWT_CHECKBOXTREE_HIDE_BOX     },
        { "COLLAPSED",    (int)(unsigned char)NEWT_CHECKBOXTREE_COLLAPSED  },
        { "EXPANDED",     (int)(unsigned char)NEWT_CHECKBOXTREE_EXPANDED   },
        { "UNSELECTED",   (int)(unsigned char)NEWT_CHECKBOXTREE_UNSELECTED },
        { "SELECTED",     (int)(unsigned char)NEWT_CHECKBOXTREE_SELECTED   },
    });

    // ── NEWT_KEY ──────────────────────────────────────────────────────────────
    // Usage: newt FormAddHotKey "$f" "${NEWT_KEY[ESCAPE]}"
    make_readonly_assoc("NEWT_KEY", {
        { "TAB",     NEWT_KEY_TAB     },
        { "ENTER",   NEWT_KEY_ENTER   },
        { "RETURN",  NEWT_KEY_RETURN  },
        { "SUSPEND", NEWT_KEY_SUSPEND },
        { "ESCAPE",  NEWT_KEY_ESCAPE  },
        { "UP",      NEWT_KEY_UP      },
        { "DOWN",    NEWT_KEY_DOWN    },
        { "LEFT",    NEWT_KEY_LEFT    },
        { "RIGHT",   NEWT_KEY_RIGHT   },
        { "BKSPC",   NEWT_KEY_BKSPC   },
        { "DELETE",  NEWT_KEY_DELETE  },
        { "HOME",    NEWT_KEY_HOME    },
        { "END",     NEWT_KEY_END     },
        { "UNTAB",   NEWT_KEY_UNTAB   },
        { "PGUP",    NEWT_KEY_PGUP    },
        { "PGDN",    NEWT_KEY_PGDN    },
        { "INSERT",  NEWT_KEY_INSERT  },
        { "RESIZE",  NEWT_KEY_RESIZE  },
        { "ERROR",   NEWT_KEY_ERROR   },
        { "F1",      NEWT_KEY_F1      },
        { "F2",      NEWT_KEY_F2      },
        { "F3",      NEWT_KEY_F3      },
        { "F4",      NEWT_KEY_F4      },
        { "F5",      NEWT_KEY_F5      },
        { "F6",      NEWT_KEY_F6      },
        { "F7",      NEWT_KEY_F7      },
        { "F8",      NEWT_KEY_F8      },
        { "F9",      NEWT_KEY_F9      },
        { "F10",     NEWT_KEY_F10     },
        { "F11",     NEWT_KEY_F11     },
        { "F12",     NEWT_KEY_F12     },
    });

    // ── NEWT_ANCHOR ───────────────────────────────────────────────────────────
    // Usage: newt GridSetField "$g" 0 0 1 "$comp" 0 0 0 0 "${NEWT_ANCHOR[LEFT]}" 0
    make_readonly_assoc("NEWT_ANCHOR", {
        { "LEFT",   NEWT_ANCHOR_LEFT   },
        { "RIGHT",  NEWT_ANCHOR_RIGHT  },
        { "TOP",    NEWT_ANCHOR_TOP    },
        { "BOTTOM", NEWT_ANCHOR_BOTTOM },
    });

    // ── NEWT_GRID_FLAG ────────────────────────────────────────────────────────
    // Usage: newt GridSetField "$g" 0 0 1 "$comp" 0 0 0 0 0 "${NEWT_GRID_FLAG[GROWX]}"
    make_readonly_assoc("NEWT_GRID_FLAG", {
        { "GROWX", NEWT_GRID_FLAG_GROWX },
        { "GROWY", NEWT_GRID_FLAG_GROWY },
    });
}
