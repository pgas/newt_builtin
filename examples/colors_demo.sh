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
# Navigation: [N]ext  [P]rev  [Q]uit — buttons or keyboard shortcuts

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

# ASCII codes for hotkeys: q=quit  n=next  p=prev
KEY_Q=$(printf '%d' "'q")
KEY_N=$(printf '%d' "'n")
KEY_P=$(printf '%d' "'p")

# ── navigation loop ───────────────────────────────────────────────────────────
page=1
quit=false
while ! $quit; do
    newt Cls
    case $page in

    # ─────────────────────────────────────────────────────────────────────────
    # Page 1: LabelSetColors – every colorset painted on its own row
    # ─────────────────────────────────────────────────────────────────────────
    1)
        newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 1/4 — LabelSetColors)"
        newt OpenWindow 4 2 65 17 "Page 1: LabelSetColors — every NEWT_COLORSET"

        cs_names=(
            ROOT BORDER WINDOW SHADOW TITLE
            BUTTON ACTBUTTON CHECKBOX ACTCHECKBOX ENTRY
            LABEL LISTBOX ACTLISTBOX TEXTBOX ACTTEXTBOX
            HELPLINE ROOTTEXT EMPTYSCALE FULLSCALE DISENTRY
            COMPACTBUTTON ACTSELLISTBOX SELLISTBOX
        )
        lbl_refs=()
        for i in "${!cs_names[@]}"; do
            name="${cs_names[$i]}"
            if (( i < 12 )); then
                col=2; row=$(( i + 1 ))
            else
                col=33; row=$(( i - 12 + 1 ))
            fi
            newt -v _lbl Label "$col" "$row" \
                "$(printf '%-14s %2d' "$name" "${NEWT_COLORSET[$name]}")"
            newt LabelSetColors "$_lbl" "${NEWT_COLORSET[$name]}"
            lbl_refs+=("$_lbl")
        done

        newt -v btn_next CompactButton 13 15 "[N]ext"
        newt -v btn_quit CompactButton 23 15 "[Q]uit"
        newt -v f Form "" "" 0
        newt FormAddComponents "$f" "${lbl_refs[@]}" "$btn_next" "$btn_quit"
        newt FormSetCurrent "$f" "$btn_next"
        newt FormAddHotKey "$f" "$KEY_Q"
        newt FormAddHotKey "$f" "$KEY_N"
        newt FormRun "$f" reason value
        newt FormDestroy "$f"
        newt PopWindow

        if [[ $reason == HOTKEY && $value == "$KEY_Q" ]] || [[ $value == "$btn_quit" ]]; then
            quit=true
        else
            (( page++ ))
        fi
        ;;

    # ─────────────────────────────────────────────────────────────────────────
    # Page 2: TextboxSetColors · EntrySetColors
    # ─────────────────────────────────────────────────────────────────────────
    2)
        newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 2/4 — TextboxSetColors / EntrySetColors)"
        newt OpenWindow 4 2 65 17 "Page 2: TextboxSetColors & EntrySetColors"

        newt -v lbl_tb  Label 3 1  "Textbox — Tab to focus it and see ACTBUTTON highlight:"
        newt -v tb Textbox 3 2 55 4 0
        newt TextboxSetText "$tb" \
"The quick brown fox jumps over the lazy dog.
Pack my box with five dozen liquor jugs.
How vexingly quick daft zebras jump!
The five boxing wizards jump quickly."
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

        newt -v btn_prev CompactButton  3 15 "[P]rev"
        newt -v btn_next CompactButton 13 15 "[N]ext"
        newt -v btn_quit CompactButton 23 15 "[Q]uit"
        newt -v f Form "" "" 0
        newt FormAddComponents "$f" \
            "$lbl_tb" "$tb" \
            "$lbl_en" "$en_norm" "$en_dis" \
            "$lbl_hint" "$btn_prev" "$btn_next" "$btn_quit"
        newt FormSetCurrent "$f" "$btn_next"
        newt FormAddHotKey "$f" "$KEY_Q"
        newt FormAddHotKey "$f" "$KEY_N"
        newt FormAddHotKey "$f" "$KEY_P"
        newt FormRun "$f" reason value
        newt FormDestroy "$f"
        newt PopWindow

        if [[ $reason == HOTKEY && $value == "$KEY_Q" ]] || [[ $value == "$btn_quit" ]]; then
            quit=true
        elif [[ $reason == HOTKEY && $value == "$KEY_P" ]] || [[ $value == "$btn_prev" ]]; then
            (( page-- ))
        else
            (( page++ ))
        fi
        ;;

    # ─────────────────────────────────────────────────────────────────────────
    # Page 3: ScaleSetColors · ScrollbarSetColors
    # ─────────────────────────────────────────────────────────────────────────
    3)
        newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 3/4 — ScaleSetColors / ScrollbarSetColors)"
        newt OpenWindow 4 2 65 17 "Page 3: ScaleSetColors & ScrollbarSetColors"

        newt -v lbl_sc Label 3 1 "Progress bar — 60% full (FULLSCALE), 40% empty (EMPTYSCALE)"
        newt -v sc Scale 3 2 50 100
        newt ScaleSetColors "$sc" "${NEWT_COLORSET[EMPTYSCALE]}" "${NEWT_COLORSET[FULLSCALE]}"
        newt ScaleSet       "$sc" 60

        newt -v lbl_sep   Label 3 3 "────────────────────────────────────────────────────────────"
        newt -v lbl_sb_hdr  Label 3 4 "Scrollbar colours (track / thumb)"
        newt -v lbl_sb  Label  3 6 "EMPTYSCALE / FULLSCALE"
        sb_items=( Alpha Beta Gamma Delta Epsilon Zeta Eta Theta Iota Kappa )
        newt -v lb3 Listbox 3 7 5 0
        for i in "${!sb_items[@]}"; do
            newt ListboxAddEntry "$lb3" "${sb_items[$i]}" "$i"
        done
        newt -v sb3 VerticalScrollbar 28 7 5 \
            "${NEWT_COLORSET[LISTBOX]}" "${NEWT_COLORSET[ACTLISTBOX]}"
        newt ScrollbarSetColors "$sb3" \
            "${NEWT_COLORSET[EMPTYSCALE]}" "${NEWT_COLORSET[FULLSCALE]}"
        newt ScrollbarSet "$sb3" 0 "$(( ${#sb_items[@]} - 1 ))"

        newt -v lbl_sb2 Label 32 6 "SHADOW / ACTBUTTON"
        newt -v lb3b Listbox 32 7 5 0
        for i in "${!sb_items[@]}"; do
            newt ListboxAddEntry "$lb3b" "${sb_items[$i]}" "$i"
        done
        newt -v sb3b VerticalScrollbar 57 7 5 \
            "${NEWT_COLORSET[LISTBOX]}" "${NEWT_COLORSET[ACTLISTBOX]}"
        newt ScrollbarSetColors "$sb3b" \
            "${NEWT_COLORSET[SHADOW]}" "${NEWT_COLORSET[ACTBUTTON]}"
        newt ScrollbarSet "$sb3b" 0 "$(( ${#sb_items[@]} - 1 ))"

        newt ComponentAddCallback "$lb3" \
            'newt -v _p ListboxGetCurrent "$lb3" && newt ScrollbarSet "$sb3" "$_p" "$(( ${#sb_items[@]} - 1 ))"'
        newt ComponentAddCallback "$lb3b" \
            'newt -v _p ListboxGetCurrent "$lb3b" && newt ScrollbarSet "$sb3b" "$_p" "$(( ${#sb_items[@]} - 1 ))"'

        newt -v btn_prev CompactButton  3 15 "[P]rev"
        newt -v btn_next CompactButton 13 15 "[N]ext"
        newt -v btn_quit CompactButton 23 15 "[Q]uit"
        newt -v f Form "" "" 0
        newt FormAddComponents "$f" \
            "$lbl_sc" "$sc" \
            "$lbl_sep" "$lbl_sb_hdr" "$lbl_sb" "$lb3" "$sb3" \
            "$lbl_sb2" "$lb3b" "$sb3b" \
            "$btn_prev" "$btn_next" "$btn_quit"
        newt FormSetCurrent "$f" "$btn_next"
        newt FormAddHotKey "$f" "$KEY_Q"
        newt FormAddHotKey "$f" "$KEY_N"
        newt FormAddHotKey "$f" "$KEY_P"
        newt FormRun "$f" reason value
        newt FormDestroy "$f"
        newt PopWindow

        if [[ $reason == HOTKEY && $value == "$KEY_Q" ]] || [[ $value == "$btn_quit" ]]; then
            quit=true
        elif [[ $reason == HOTKEY && $value == "$KEY_P" ]] || [[ $value == "$btn_prev" ]]; then
            (( page-- ))
        else
            (( page++ ))
        fi
        ;;

    # ─────────────────────────────────────────────────────────────────────────
    # Page 4: SetColor live patching
    # ─────────────────────────────────────────────────────────────────────────
    4)
        newt DrawRootText 2 0 "Screen: ${COLS}x${ROWS}   (Page 4/4 — SetColor live patching)"
        newt OpenWindow 4 2 65 17 "Page 4: SetColor — live colorset patching"

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

        newt -v pb_root  CompactButton 3  9  "Patch ROOT → red on black"
        newt -v pb_title CompactButton 3  11 "Patch TITLE → cyan on black"
        newt -v pb_btn   CompactButton 3  13 "Patch BUTTON → green on black"
        newt -v btn_prev CompactButton  3 15 "[P]rev"
        newt -v btn_quit CompactButton 23 15 "[Q]uit"

        newt -v f Form "" "" 0
        newt FormAddComponents "$f" \
            "$lbl4a" "$lbl4b" "$lbl4_root" "$lbl4c" "$lbl4_title" \
            "$lbl4d" "$lbl4_btn" \
            "$pb_root" "$pb_title" "$pb_btn" "$btn_prev" "$btn_quit"
        newt FormSetCurrent "$f" "$btn_quit"
        newt FormAddHotKey "$f" "$KEY_Q"
        newt FormAddHotKey "$f" "$KEY_P"

        while true; do
            newt FormRun "$f" reason value
            if [[ $reason == HOTKEY && $value == "$KEY_Q" ]] || [[ $value == "$btn_quit" ]]; then
                quit=true; break
            elif [[ $reason == HOTKEY && $value == "$KEY_P" ]] || [[ $value == "$btn_prev" ]]; then
                (( page-- )); break
            else
                case "$value" in
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
                esac
                newt Refresh
            fi
        done

        newt FormDestroy "$f"
        newt PopWindow
        ;;

    esac
done
