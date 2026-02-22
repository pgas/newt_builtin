#!/bin/bash
# colors_demo.sh — interactive gallery of every newt colour API
#
# Demonstrates:
#   SetColors         – replace the entire palette at once
#   SetColor          – patch a single colorset (fg/bg) live
#   LabelSetColors    – apply any NEWT_COLORSET to a label
#   TextboxSetColors  – normal / active textbox colours
#   EntrySetColors    – normal / disabled entry colours
#   ScaleSetColors    – empty / full progress bar colours
#   ScrollbarSetColors– normal / thumb scrollbar colours
#
# Each page stands alone; press Next / the displayed button to advance.

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

newt Init
trap 'newt Finished' EXIT
newt Cls

newt GetScreenSize COLS ROWS

# ─────────────────────────────────────────────────────────────────────────────
# Page 1: LabelSetColors – every colorset painted on its own row
# ─────────────────────────────────────────────────────────────────────────────
newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 1/4 — LabelSetColors)"
newt OpenWindow 4 1 65 22 "Page 1: LabelSetColors — every NEWT_COLORSET"

declare -a cs_names=(
    ROOT BORDER WINDOW SHADOW TITLE
    BUTTON ACTBUTTON CHECKBOX ACTCHECKBOX ENTRY
    LABEL LISTBOX ACTLISTBOX TEXTBOX ACTTEXTBOX
    HELPLINE ROOTTEXT EMPTYSCALE FULLSCALE DISENTRY
    COMPACTBUTTON ACTSELLISTBOX SELLISTBOX
)

row=1
declare -a lbl_refs=()
for name in "${cs_names[@]}"; do
    newt -v _lbl Label 3 "$row" "$(printf '%-14s  colorset %-2d' \
        "$name" "${NEWT_COLORSET[$name]}")"
    newt LabelSetColors "$_lbl" "${NEWT_COLORSET[$name]}"
    lbl_refs+=("$_lbl")
    (( row++ ))
done

newt -v btn1 Button 48 19 "Next >"
newt -v f1 Form NULL NULL 0
newt FormAddComponents "$f1" "${lbl_refs[@]}" "$btn1"
newt RunForm "$f1"
newt FormDestroy "$f1"
newt PopWindow

# ─────────────────────────────────────────────────────────────────────────────
# Page 2: TextboxSetColors · EntrySetColors
# ─────────────────────────────────────────────────────────────────────────────
newt Cls
newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 2/4 — TextboxSetColors / EntrySetColors)"
newt OpenWindow 4 1 65 20 "Page 2: TextboxSetColors & EntrySetColors"

newt -v lbl_tb  Label 3 1  "Textbox: normal=WINDOW active=ACTBUTTON"
newt -v tb Textbox 3 2 55 4 0
newt TextboxSetText "$tb" \
"Line 1 — normal colourset (WINDOW)
Line 2 — the active colourset (ACTBUTTON) is used
Line 3 — when the textbox has focus; tab to see it.
Line 4 — four lines of text."
newt TextboxSetColors "$tb" \
    "${NEWT_COLORSET[WINDOW]}" "${NEWT_COLORSET[ACTBUTTON]}"

newt -v lbl_en  Label 3 7  "Entry: normal=ENTRY  disabled=DISENTRY"
newt -v en_norm Entry 3 8 "I am a normal entry" 50
newt EntrySetColors "$en_norm" \
    "${NEWT_COLORSET[ENTRY]}" "${NEWT_COLORSET[ACTBUTTON]}"

newt -v en_dis  Entry 3 10 "I am a disabled entry (DISENTRY colour)" 50
newt EntrySetColors "$en_dis" \
    "${NEWT_COLORSET[DISENTRY]}" "${NEWT_COLORSET[DISENTRY]}"
newt EntrySetFlags  "$en_dis" "${NEWT_FLAG[DISABLED]}" 0

newt -v lbl_hint Label 3 12 \
    "Tab between widgets to see focus (active) colours change."

newt -v btn2 Button 3 14 "Next >"
newt -v f2 Form NULL NULL 0
newt FormAddComponents "$f2" \
    "$lbl_tb" "$tb" \
    "$lbl_en" "$en_norm" "$en_dis" \
    "$lbl_hint" "$btn2"
newt RunForm "$f2"
newt FormDestroy "$f2"
newt PopWindow

# ─────────────────────────────────────────────────────────────────────────────
# Page 3: ScaleSetColors · ScrollbarSetColors
# ─────────────────────────────────────────────────────────────────────────────
newt Cls
newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 3/4 — ScaleSetColors / ScrollbarSetColors)"
newt OpenWindow 4 1 65 20 "Page 3: ScaleSetColors & ScrollbarSetColors"

newt -v lbl_sc Label 3 1 "Scale: empty=WINDOW  full=FULLSCALE"
newt -v sc Scale 3 2 50 100
newt ScaleSetColors "$sc" "${NEWT_COLORSET[WINDOW]}" "${NEWT_COLORSET[FULLSCALE]}"
newt ScaleSet       "$sc" 60

newt -v lbl_sc2 Label 3 4 "Scale: empty=EMPTYSCALE  full=ACTBUTTON"
newt -v sc2 Scale 3 5 50 100
newt ScaleSetColors "$sc2" "${NEWT_COLORSET[EMPTYSCALE]}" "${NEWT_COLORSET[ACTBUTTON]}"
newt ScaleSet       "$sc2" 30

newt -v lbl_sb Label 3 7 "Scrollbar (right): normal=LISTBOX  thumb=ACTLISTBOX"
declare -a sb_items=( Alpha Beta Gamma Delta Epsilon Zeta Eta Theta Iota Kappa )
newt -v lb3 Listbox 3 8 5 0
for i in "${!sb_items[@]}"; do
    newt ListboxAddEntry "$lb3" "${sb_items[$i]}" "$i"
done
newt -v sb3 VerticalScrollbar 28 8 5 \
    "${NEWT_COLORSET[LISTBOX]}" "${NEWT_COLORSET[ACTLISTBOX]}"
newt ScrollbarSetColors "$sb3" \
    "${NEWT_COLORSET[LISTBOX]}" "${NEWT_COLORSET[ACTLISTBOX]}"
newt ScrollbarSet "$sb3" 0 "$(( ${#sb_items[@]} - 1 ))"

newt -v lbl_sb2 Label 32 7 "Scrollbar: normal=TEXTBOX  thumb=FULLSCALE"
newt -v lb3b Listbox 32 8 5 0
for i in "${!sb_items[@]}"; do
    newt ListboxAddEntry "$lb3b" "${sb_items[$i]}" "$i"
done
newt -v sb3b VerticalScrollbar 57 8 5 \
    "${NEWT_COLORSET[TEXTBOX]}" "${NEWT_COLORSET[FULLSCALE]}"
newt ScrollbarSetColors "$sb3b" \
    "${NEWT_COLORSET[TEXTBOX]}" "${NEWT_COLORSET[FULLSCALE]}"
newt ScrollbarSet "$sb3b" 0 "$(( ${#sb_items[@]} - 1 ))"

# Sync scrollbars via callbacks
newt ComponentAddCallback "$lb3" \
    'newt -v _p ListboxGetCurrent "$lb3" && newt ScrollbarSet "$sb3" "$_p" "$(( ${#sb_items[@]} - 1 ))"'
newt ComponentAddCallback "$lb3b" \
    'newt -v _p ListboxGetCurrent "$lb3b" && newt ScrollbarSet "$sb3b" "$_p" "$(( ${#sb_items[@]} - 1 ))"'

newt -v btn3 Button 3 15 "Next >"
newt -v f3 Form NULL NULL 0
newt FormAddComponents "$f3" \
    "$lbl_sc" "$sc" "$lbl_sc2" "$sc2" \
    "$lbl_sb" "$lb3" "$sb3" \
    "$lbl_sb2" "$lb3b" "$sb3b" \
    "$btn3"
newt RunForm "$f3"
newt FormDestroy "$f3"
newt PopWindow

# ─────────────────────────────────────────────────────────────────────────────
# Page 4: SetColor live patching
# ─────────────────────────────────────────────────────────────────────────────
newt Cls
newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 4/4 — SetColor live patching)"
newt OpenWindow 4 1 65 20 "Page 4: SetColor — live colorset patching"

newt -v lbl4a Label 3 1  "Watch each colorset change as you press the buttons."
newt -v lbl4b Label 3 3  "ROOT colorset:"
newt -v lbl4_root Label 22 3 "white on blue (default)"
newt LabelSetColors "$lbl4_root" "${NEWT_COLORSET[ROOT]}"

newt -v lbl4c Label 3 5  "TITLE colorset:"
newt -v lbl4_title Label 22 5 "yellow on blue (default)"
newt LabelSetColors "$lbl4_title" "${NEWT_COLORSET[TITLE]}"

newt -v lbl4d Label 3 7  "BUTTON colorset:"
newt -v lbl4_btn Label 22 7 "black on cyan (default)"
newt LabelSetColors "$lbl4_btn" "${NEWT_COLORSET[BUTTON]}"

# Buttons to patch each colorset live
newt -v pb_root  Button 3  10 "Patch ROOT→red on black"
newt -v pb_title Button 3  12 "Patch TITLE→cyan on black"
newt -v pb_btn   Button 3  14 "Patch BUTTON→green on black"
newt -v pb_done  Button 45 14 "Done"

newt -v f4 Form NULL NULL 0
newt FormAddComponents "$f4" \
    "$lbl4a" "$lbl4b" "$lbl4_root" "$lbl4c" "$lbl4_title" \
    "$lbl4d" "$lbl4_btn" \
    "$pb_root" "$pb_title" "$pb_btn" "$pb_done"

while true; do
    newt -v _pressed RunForm "$f4"
    case "$_pressed" in
        "$pb_root")
            newt SetColor "${NEWT_COLORSET[ROOT]}" red black
            newt LabelSetText "$lbl4_root" "red on black (patched)"
            ;;
        "$pb_title")
            newt SetColor "${NEWT_COLORSET[TITLE]}" cyan black
            newt LabelSetText "$lbl4_title" "cyan on black (patched)"
            ;;
        "$pb_btn")
            newt SetColor "${NEWT_COLORSET[BUTTON]}" green black
            newt LabelSetText "$lbl4_btn" "green on black (patched)"
            ;;
        "$pb_done") break ;;
    esac
    newt Refresh
done

newt FormDestroy "$f4"
newt PopWindow
