#!/bin/bash
# peanuts.sh – bash port of newt/peanuts.py
#
# Demonstrates: Listbox, Radiobutton, CompactButton (ButtonBar), CheckboxTree
# in a single form.  Mirrors the Python peanuts.py demo that ships with
# libnewt.
#
# Usage: bash peanuts.sh

# ── locate and load the newt builtin ─────────────────────────────────────────
for lib in libs/ ../libs/ build/src/ ../build/src/; do
    if [[ -f $lib/newt.so ]]; then
        enable -f "$lib"/newt.so newt
        break
    fi
done

if ! type newt &>/dev/null; then
    echo >&2 "unable to load the newt builtin"
    exit 1
fi

# ── main ─────────────────────────────────────────────────────────────────────
if newt Init; then
    trap 'newt Finished' EXIT
    newt Cls
    newt CenteredWindow 36 18 "My Test"

    # ── Listbox (height=3, width=20, returnExit=1) ────────────────────────────
    newt -v li Listbox 2 1 3 "${NEWT_FLAG[RETURNEXIT]}"
    newt ListboxSetWidth "$li" 20
    newt ListboxAppendEntry "$li" "First"  1
    newt ListboxAppendEntry "$li" "Second" 2
    newt ListboxAppendEntry "$li" "Third"  3

    # ── RadioBar: This / Default(selected) / That ─────────────────────────────
    newt -v rb0 Radiobutton 2 5 "This"    0 ""
    newt -v rb1 Radiobutton 2 6 "Default" 1 "$rb0"
    newt -v rb2 Radiobutton 2 7 "That"    0 "$rb1"

    # ── CheckboxTree (height=5, scroll=1) ─────────────────────────────────────
    #   Colors (root index 0)
    #     Red / Yellow / Blue
    #   Flavors (root index 1 – no children)
    #   Numbers (root index 2)
    #     1 / 2 / 3
    #   Names / Months / Events (root indices 3-5, no children)
    newt -v ct CheckboxTree 2 9 5 "${NEWT_FLAG[SCROLL]}"

    newt CheckboxTreeAddItem "$ct" "Colors"  1  0 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "Red"     2  0  0 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "Yellow"  3  0  0 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "Blue"    4  0  0 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "Flavors" 5  0 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "Numbers" 6  0 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "1"       7  0  2 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "2"       8  0  2 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "3"       9  0  2 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "Names"   10 0 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "Months"  11 0 "${NEWT_ARG[APPEND]}"
    newt CheckboxTreeAddItem "$ct" "Events"  12 0 "${NEWT_ARG[APPEND]}"

    # ── ButtonBar: Ok / Cancel ────────────────────────────────────────────────
    newt -v btn_ok     CompactButton 2  15 "Ok"
    newt -v btn_cancel CompactButton 11 15 "Cancel"

    # ── form ──────────────────────────────────────────────────────────────────
    newt -v f Form "" "" 0
    newt FormAddComponents "$f" \
        "$li" "$rb0" "$rb1" "$rb2" "$ct" "$btn_ok" "$btn_cancel"

    newt -v result RunForm "$f"

    # Collect values before FormDestroy frees the components.
    newt -v li_cur ListboxGetCurrent "$li"
    newt -v rb_cur RadioGetCurrent   "$rb0"
    newt CheckboxTreeGetSelection "$ct" ct_sel

    newt FormDestroy "$f"
fi

# ── results (mirrors peanuts.py output) ───────────────────────────────────────
echo "result: $result"

echo "listbox: $li_cur"

if   [[ "$rb_cur" == "$rb0" ]]; then echo "rb: this"
elif [[ "$rb_cur" == "$rb1" ]]; then echo "rb: default"
elif [[ "$rb_cur" == "$rb2" ]]; then echo "rb: that"
else                                  echo "rb: (unknown)"
fi

if   [[ "$result" == "$btn_ok"     ]]; then echo "bb: ok"
elif [[ "$result" == "$btn_cancel" ]]; then echo "bb: cancel"
else                                         echo "bb: (other)"
fi

echo -n "checkboxtree: "
if (( ct_sel == 0 )); then
    echo "(none selected)"
else
    keys=()
    for (( i = 0; i < ct_sel; i++ )); do
        varname="ct_sel_${i}"
        keys+=( "${!varname}" )
    done
    echo "${keys[*]}"
fi
