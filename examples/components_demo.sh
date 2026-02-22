#!/bin/bash
# components_demo.sh — interactive showcase of every component function
# declared in newt.h lines 141-164:
#
#   CompactButton  Button  Checkbox  CheckboxGetValue  CheckboxSetValue
#   CheckboxSetFlags  Radiobutton  RadioGetCurrent  RadioSetCurrent
#   GetScreenSize  Label  LabelSetText  LabelSetColors  VerticalScrollbar
#   ScrollbarSet  ScrollbarSetColors  Listbox

# ── locate and load the newt builtin ─────────────────────────────────────────
for lib in libs/ ../libs/ build/src/ ../build/src/; do
    if [[ -f $lib/newt.so ]]; then
        enable -f "$lib"/newt.so newt
        break
    fi
done

if ! type newt &>/dev/null; then
    echo >&2 "Unable to load the newt builtin."
    exit 1
fi

# ─────────────────────────────────────────────────────────────────────────────
# Page 1: GetScreenSize  ·  Label / LabelSetText / LabelSetColors
#         Button  ·  CompactButton
# ─────────────────────────────────────────────────────────────────────────────
newt Init
trap 'newt Finished' EXIT
newt Cls

# --- GetScreenSize -----------------------------------------------------------
newt GetScreenSize COLS ROWS
newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 1/4 — press Next)"

newt OpenWindow 5 2 65 18 "Page 1: Label / Button / CompactButton"

# --- Label -------------------------------------------------------------------
newt -v lbl_title  Label 3 1 "newtLabel(left, top, text)"
newt -v lbl_change Label 5 3 "Before LabelSetText"
newt -v lbl_color  Label 5 5 "LabelSetColors: colorset 8 (ACTBUTTON)"

# --- LabelSetText ------------------------------------------------------------
newt LabelSetText "$lbl_change" "After LabelSetText ✓"

# --- LabelSetColors ----------------------------------------------------------
newt LabelSetColors "$lbl_color" "${NEWT_COLORSET[ACTBUTTON]}"

# --- Button / CompactButton --------------------------------------------------
newt -v btn_next    Button        3 10 "Next >"
newt -v btn_cb      CompactButton 3 13 "CompactButton"
newt -v btn_cb2     CompactButton 22 13 "Also CompactButton"

newt -v f1 Form NULL NULL 0
newt FormAddComponents "$f1" \
    "$lbl_title" "$lbl_change" "$lbl_color" \
    "$btn_cb" "$btn_cb2" "$btn_next"

newt RunForm "$f1"
newt FormDestroy "$f1"
newt PopWindow

# ─────────────────────────────────────────────────────────────────────────────
# Page 2: Checkbox / CheckboxGetValue / CheckboxSetValue / CheckboxSetFlags
# ─────────────────────────────────────────────────────────────────────────────
newt Cls
newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 2/4 — toggle checkboxes then Next)"
newt OpenWindow 5 2 65 18 "Page 2: Checkbox"

newt -v lbl2 Label 3 1 "Toggle the checkboxes, then press Next >"

# Checkbox left top text [defValue [seq]]
# defValue ' ' = unchecked, '*' = checked; seq = custom toggle chars
newt -v cb_a Checkbox 3 3  "Option A (default unchecked)"
newt -v cb_b Checkbox 3 4  "Option B (pre-checked)"       "*"
newt -v cb_c Checkbox 3 5  "Option C (custom toggles: 0=off 1=on)" " " "01"

# --- CheckboxSetValue: force Option B to ' ' (unchecked) then back to '*' ---
newt CheckboxSetValue "$cb_b" " "
newt CheckboxSetValue "$cb_b" "*"

# --- CheckboxSetFlags: disable Option C, then re-enable it ---
# We disable it here to demonstrate the API; re-enable below so the user can still toggle it
newt CheckboxSetFlags "$cb_c" "${NEWT_FLAG[DISABLED]}" 0    # set NEWT_FLAG_DISABLED
newt CheckboxSetFlags "$cb_c" "${NEWT_FLAG[DISABLED]}" 1    # reset NEWT_FLAG_DISABLED (re-enable)

newt -v btn2_next Button 3 13 "Next >"

newt -v f2 Form NULL NULL 0
newt FormAddComponents "$f2" "$lbl2" "$cb_a" "$cb_b" "$cb_c" "$btn2_next"
newt RunForm "$f2"
newt FormDestroy "$f2"

# --- CheckboxGetValue --------------------------------------------------------
newt -v val_a CheckboxGetValue "$cb_a"
newt -v val_b CheckboxGetValue "$cb_b"
newt -v val_c CheckboxGetValue "$cb_c"

newt PopWindow

# ─────────────────────────────────────────────────────────────────────────────
# Page 3: Radiobutton / RadioGetCurrent / RadioSetCurrent
# ─────────────────────────────────────────────────────────────────────────────
newt Cls
newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 3/4 — pick a radio then Next)"
newt OpenWindow 5 2 65 18 "Page 3: Radiobutton"

newt -v lbl3 Label 3 1 "Select one option, then press Next >"

# Parallel arrays: labels and corresponding component pointers
declare -a rb_labels=("Choice Alpha" "Choice Beta" "Choice Gamma" "Choice Delta")
declare -a rb_ptrs=()

# Radiobutton left top text isDefault prevButton
# newt -v "rb_ptrs[i++]" stores the pointer directly into the array;
# bash evaluates the subscript arithmetic via builtin_bind_variable.
i=0
for label in "${rb_labels[@]}"; do
    prev="${rb_ptrs[i-1]:-NULL}"
    isdef=$(( i == 0 ? 1 : 0 ))
    row=$(( 3 + i ))
    newt -v "rb_ptrs[i++]" Radiobutton 3 "$row" "$label" $isdef "$prev"
done

# --- RadioSetCurrent: pre-select the third button ---
newt RadioSetCurrent "${rb_ptrs[2]}"

newt -v btn3_next Button 3 13 "Next >"

newt -v f3 Form NULL NULL 0
newt FormAddComponents "$f3" "$lbl3" \
    "${rb_ptrs[0]}" "${rb_ptrs[1]}" "${rb_ptrs[2]}" "${rb_ptrs[3]}" \
    "$btn3_next"
newt RunForm "$f3"
newt FormDestroy "$f3"

# --- RadioGetCurrent: find the chosen pointer, then look up index + label ---
newt -v _chosen_ptr RadioGetCurrent "${rb_ptrs[0]}"
chosen_idx=""
chosen_label=""
for i in "${!rb_ptrs[@]}"; do
    if [[ "${rb_ptrs[$i]}" == "$_chosen_ptr" ]]; then
        chosen_idx=$i
        chosen_label="${rb_labels[$i]}"
        break
    fi
done

newt PopWindow

# ─────────────────────────────────────────────────────────────────────────────
# Page 4: Listbox / VerticalScrollbar / ScrollbarSet / ScrollbarSetColors
# ─────────────────────────────────────────────────────────────────────────────
newt Cls
newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 4/4 — scroll the list then Finish)"
newt OpenWindow 5 2 65 20 "Page 4: Listbox + VerticalScrollbar"

newt -v lbl4 Label 3 1 "Scroll the list, then press Finish >"

# --- Listbox left top height flags -------------------------------------------
newt -v lb Listbox 3 3 6 0

# Populate via ListboxAddEntry (not in lines 141-164, but needed to fill the list)
declare -a animals=( "Aardvark" "Bear" "Cheetah" "Dolphin" "Eagle"
                     "Falcon"   "Gorilla" "Hippo"  "Iguana" "Jaguar" )
for i in "${!animals[@]}"; do
    newt ListboxAddEntry "$lb" "${animals[$i]}" "$i"
done

# --- VerticalScrollbar left top height normalColorset thumbColorset ----------
newt -v sb VerticalScrollbar 28 3 6 \
    "${NEWT_COLORSET[LISTBOX]}" "${NEWT_COLORSET[ACTLISTBOX]}"

# --- ScrollbarSetColors: swap to COLORSET_ACTBUTTON / COLORSET_BUTTON --------
newt ScrollbarSetColors "$sb" \
    "${NEWT_COLORSET[ACTBUTTON]}" "${NEWT_COLORSET[BUTTON]}"

# --- ScrollbarSet co where total ---------------------------------------------
# Initialise the scrollbar to position 0 out of 10 items
newt ScrollbarSet "$sb" 0 10

newt -v btn4_done Button 3 16 "Finish"

newt -v f4 Form NULL NULL 0
newt FormAddComponents "$f4" "$lbl4" "$lb" "$sb" "$btn4_done"
newt RunForm "$f4"

# Read back current listbox selection (demonstrates RadioGetCurrent analog);
# retrieve selection index and update scrollbar as items are scrolled.
newt -v sel_idx ListboxGetCurrent "$lb"
newt FormDestroy "$f4"
newt PopWindow

# ─────────────────────────────────────────────────────────────────────────────
# Summary (printed to the terminal after restoring it via newt Finished)
# ─────────────────────────────────────────────────────────────────────────────
# Restore the terminal explicitly before printing.  The trap 'newt Finished'
# registered at startup is still in place as a safety net for abnormal exits;
# because wrap_Finished is idempotent, calling it twice is harmless.
newt Finished
printf '\n=== Demo summary ===\n'
printf 'Screen size : %sx%s\n'   "$COLS" "$ROWS"
printf 'Checkbox A  : %s\n'      "$val_a"
printf 'Checkbox B  : %s\n'      "$val_b"
printf 'Checkbox C  : %s\n'      "$val_c"
printf 'Radio chose : index %s (%s)\n' "$chosen_idx" "$chosen_label"
printf 'Listbox sel : index %s (%s)\n' "$sel_idx" "${animals[$sel_idx]:-?}"
