#!/bin/bash
# popcorn.sh – bash port of newt/popcorn.py
#
# Demonstrates: TextboxReflowed, Listbox (insert/delete), Button, Entry,
# Label, Checkbox, Radiobutton, CheckboxTree, multiple forms, EntryWindow
# and ListboxChoiceWindow equivalents.  Mirrors popcorn.py from libnewt.
#
# Usage: bash popcorn.sh

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

# ── helper: bail on usage ─────────────────────────────────────────────────────
die() { echo >&2 "$*"; exit 1; }

# ─────────────────────────────────────────────────────────────────────────────
newt Init || die "newt Init failed"
trap 'newt Finished' EXIT
newt Cls

# ═══════════════════════════════════════════════════════════════════════════════
# 1.  EntryWindow equivalent
#     Python: foo = EntryWindow(screen, 'Title',
#                 'This is some text for the entry window',
#                 ['prompt', 'more', 'info'])
# ═══════════════════════════════════════════════════════════════════════════════
newt CenteredWindow 48 13 "Title"

# Description (reflowed to width 42)
newt -v _ew_tb TextboxReflowed 2 1 \
    "This is some text for the entry window" 42 3 3 0

# Prompt labels + entry fields (one per row)
newt -v _ew_l0 Label 2 3 "prompt:"
newt -v _ew_e0 Entry 12 3 "" 20 "${NEWT_FLAG[SCROLL]}"

newt -v _ew_l1 Label 2 5 "more: "
newt -v _ew_e1 Entry 12 5 "" 20 "${NEWT_FLAG[SCROLL]}"

newt -v _ew_l2 Label 2 7 "info: "
newt -v _ew_e2 Entry 12 7 "" 20 "${NEWT_FLAG[SCROLL]}"

# Buttons
newt -v _ew_ok     CompactButton 10 9 "Ok"
newt -v _ew_cancel CompactButton 22 9 "Cancel"

newt -v _ew_form Form
newt FormAddComponents "$_ew_form" \
    "$_ew_tb" "$_ew_l0" "$_ew_e0" "$_ew_l1" "$_ew_e1" "$_ew_l2" "$_ew_e2" \
    "$_ew_ok" "$_ew_cancel"

newt -v _ew_result RunForm "$_ew_form"
newt -v _foo_prompt EntryGetValue  "$_ew_e0"
newt -v _foo_more   EntryGetValue  "$_ew_e1"
newt -v _foo_info   EntryGetValue  "$_ew_e2"
if   [[ "$_ew_result" == "$_ew_ok"     ]]; then _foo_btn="ok"
elif [[ "$_ew_result" == "$_ew_cancel" ]]; then _foo_btn="cancel"
else                                            _foo_btn="other"
fi
newt FormDestroy "$_ew_form"
newt PopWindow

# ═══════════════════════════════════════════════════════════════════════════════
# 2.  ListboxChoiceWindow equivalent
#     Python: lbcw = ListboxChoiceWindow(screen, 'Title 2',
#                 'Choose one item from the list below:',
#                 ('One','Two','Three','Four','Five'), default=2,
#                 help="Help for a listbox")
# ═══════════════════════════════════════════════════════════════════════════════
newt CenteredWindow 46 16 "Title 2"

newt -v _lc_tb TextboxReflowed 2 1 \
    "Choose one item from the list below:" 40 3 3 0

newt -v _lc_lb Listbox 2 4 5 "${NEWT_FLAG[RETURNEXIT]}"
newt ListboxSetWidth "$_lc_lb" 38
newt ListboxAppendEntry "$_lc_lb" "One"   1
newt ListboxAppendEntry "$_lc_lb" "Two"   2
newt ListboxAppendEntry "$_lc_lb" "Three" 3
newt ListboxAppendEntry "$_lc_lb" "Four"  4
newt ListboxAppendEntry "$_lc_lb" "Five"  5
newt ListboxSetCurrentByKey "$_lc_lb" 3   # default=2 (0-based) → "Three" (key=3)

newt -v _lc_ok     CompactButton 8  11 "Ok"
newt -v _lc_cancel CompactButton 20 11 "Cancel"

newt PushHelpLine "Help for a listbox"
newt -v _lc_form Form
newt FormAddComponents "$_lc_form" \
    "$_lc_tb" "$_lc_lb" "$_lc_ok" "$_lc_cancel"

newt -v _lc_result RunForm "$_lc_form"
newt -v _lbcw_item ListboxGetCurrent "$_lc_lb"
if   [[ "$_lc_result" == "$_lc_ok"     ]]; then _lbcw_btn="ok"
elif [[ "$_lc_result" == "$_lc_cancel" ]]; then _lbcw_btn="cancel"
else                                            _lbcw_btn="other"
fi
newt FormDestroy "$_lc_form"
newt PopHelpLine
newt PopWindow

# ═══════════════════════════════════════════════════════════════════════════════
# 3.  Main form  (sg+g grid layout from popcorn.py, run as a plain form)
#
#   Widgets:
#     t   TextboxReflowed  "Some text which needs to be wrapped at a good place."
#     li  Listbox          height=5, returnExit=1  (First/Second; "Another"
#                          inserted then deleted to show insert/delete)
#     b   Button           "Button"
#     e   Entry            width=15  text="Entry"
#     l   Label            "label"
#     cb  Checkbox         "checkbox"
#     r1  Radiobutton      "Radio 1"  (default, group anchor)
#     r2  Radiobutton      "Radio 2"
#
#   Form help text: "This is some help"
# ═══════════════════════════════════════════════════════════════════════════════

# -- TextboxReflowed
newt -v t TextboxReflowed 2 1 \
    "Some text which needs to be wrapped at a good place." 25 5 10 0

# -- Listbox (height=5, width=20, returnExit=1)
newt -v li Listbox 2 5 5 "${NEWT_FLAG[RETURNEXIT]}"
newt ListboxSetWidth "$li" 20
newt ListboxAppendEntry "$li" "First"   1
newt ListboxAppendEntry "$li" "Second"  2
# Demonstrate insert and delete: "Another" inserted before First, then removed.
newt ListboxInsertEntry "$li" "Another" 3 1   # insert key=3 before key=1
newt ListboxDeleteEntry "$li" 3               # delete key=3 → back to First/Second

# -- Remaining widgets
newt -v b  Button        2  11 "Button"
newt -v e  Entry        21  11 "Entry" 15 "${NEWT_FLAG[SCROLL]}"
newt -v l  Label         2  13 "label"
newt -v cb Checkbox     21  13 "checkbox"
newt -v r1 Radiobutton   2  15 "Radio 1" 1 ""
newt -v r2 Radiobutton  21  15 "Radio 2" 0 "$r1"

newt PushHelpLine "This is some help"
newt CenteredWindow 50 18 "title"

newt -v f Form "" "" 0
newt FormAddComponents "$f" "$li" "$b" "$e" "$l" "$cb" "$r1" "$r2" "$t"
newt -v res RunForm "$f"

# Collect values before FormDestroy
newt -v e_val  EntryGetValue "$e"
newt -v cb_val CheckboxGetValue "$cb"
newt -v rb_cur RadioGetCurrent "$r1"
newt -v li_cur ListboxGetCurrent "$li"
r1_sel=0; [[ "$rb_cur" == "$r1" ]] && r1_sel=1

newt FormDestroy "$f"
newt PopWindow
newt PopHelpLine

# ═══════════════════════════════════════════════════════════════════════════════
# 4.  CheckboxTree GridForm
#     Python: g = GridForm(screen, "Tree", 1, 2)
#             g.add(ct, 0, 0, …)
#             g.add(Button("Ok"), 0, 1)
#             g.runOnce()
# ═══════════════════════════════════════════════════════════════════════════════
newt CenteredWindow 38 14 "Tree"

newt -v ct CheckboxTree 2 1 5 "${NEWT_FLAG[SCROLL]}"

# Colors (root 0) → Red, Yellow, Blue
newt CheckboxTreeAddItem "$ct" "Colors"    1  0 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Red"       2  0  0 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Yellow"    3  0  0 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Blue"      4  0  0 "${NEWT_ARG[APPEND]}"
# Flavors (root 1) → Vanilla, Chocolate, Stawberry
newt CheckboxTreeAddItem "$ct" "Flavors"   5  0 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Vanilla"   6  0  1 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Chocolate" 7  0  1 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Stawberry" 8  0  1 "${NEWT_ARG[APPEND]}"
# Numbers (root 2) → 1, 2, 3
newt CheckboxTreeAddItem "$ct" "Numbers"   9  0 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "1"         10 0  2 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "2"         11 0  2 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "3"         12 0  2 "${NEWT_ARG[APPEND]}"
# Names (root 3) → Matt, Shawn, Wilson
newt CheckboxTreeAddItem "$ct" "Names"     13 0 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Matt"      14 0  3 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Shawn"     15 0  3 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Wilson"    16 0  3 "${NEWT_ARG[APPEND]}"
# Months (root 4) → February, August, September
newt CheckboxTreeAddItem "$ct" "Months"    17 0 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "February"  18 0  4 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "August"    19 0  4 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "September" 20 0  4 "${NEWT_ARG[APPEND]}"
# Events (root 5) → Christmas, Labor Day, My Vacation
newt CheckboxTreeAddItem "$ct" "Events"    21 0 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Christmas"   22 0  5 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "Labor Day"   23 0  5 "${NEWT_ARG[APPEND]}"
newt CheckboxTreeAddItem "$ct" "My Vacation" 24 0  5 "${NEWT_ARG[APPEND]}"

newt -v btn_ok_ct Button 2 8 "Ok"

newt -v f_ct Form
newt FormAddComponents "$f_ct" "$ct" "$btn_ok_ct"
newt RunForm "$f_ct"

# Collect CheckboxTree results before destroying
newt -v ct_cur CheckboxTreeGetCurrent "$ct"
newt CheckboxTreeGetSelection "$ct" ct_sel

newt FormDestroy "$f_ct"
newt PopWindow

# Explicit Finished before printing (trap is a safety net).
newt Finished

# ═══════════════════════════════════════════════════════════════════════════════
# Results  (mirrors popcorn.py final print block)
# ═══════════════════════════════════════════════════════════════════════════════
echo "val $e_val"
echo "check $cb_val"
echo "r1 $r1_sel"
echo "listbox $li_cur"
echo "$res"

echo "($( [[ "$_foo_btn" == ok ]] && echo "'ok'" || echo "'cancel'" ), \
('$_foo_prompt', '$_foo_more', '$_foo_info'))"

echo "lbcw ($_lbcw_btn, $_lbcw_item)"

echo -n "ct selected "
if (( ct_sel == 0 )); then
    echo "()"
else
    keys=()
    for (( i = 0; i < ct_sel; i++ )); do
        varname="ct_sel_${i}"
        keys+=( "${!varname}" )
    done
    echo "(${keys[*]})"
fi

echo "ct current $ct_cur"
