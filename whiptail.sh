#!/bin/bash
# whiptail.sh — drop-in replacement for whiptail(1) using the newt bash builtin
#
# Usage:  whiptail.sh [options] --boxtype text height width [box-args...]
#
# Supports the same options and box types as whiptail(1):
#   --msgbox, --yesno, --infobox, --inputbox, --passwordbox,
#   --textbox, --menu, --checklist, --radiolist, --gauge
#
# Requires: the newt bash builtin (newt.so) loadable via enable -f.

set -uo pipefail

# ── locate and load the newt builtin ─────────────────────────────────────────
_whiptail_load_builtin() {
    if type newt &>/dev/null; then
        return 0
    fi
    local dir
    for dir in \
        "${WHIPTAIL_NEWT_SO:+${WHIPTAIL_NEWT_SO%/*}}" \
        "build/src" "../build/src" "libs" "../libs" \
        "/usr/lib/bash" "/usr/local/lib/bash"
    do
        if [[ -n "$dir" && -f "$dir/newt.so" ]]; then
            enable -f "$dir/newt.so" newt
            return 0
        fi
    done
    echo >&2 "whiptail.sh: cannot find newt.so; set WHIPTAIL_NEWT_SO or install it."
    exit 255
}
_whiptail_load_builtin

# ── constants ────────────────────────────────────────────────────────────────
# After loading the builtin, NEWT_FLAG[] and NEWT_KEY[] are available.
readonly FLAG_RETURNEXIT=${NEWT_FLAG[RETURNEXIT]:-1}
readonly FLAG_SCROLL=${NEWT_FLAG[SCROLL]:-4}
readonly FLAG_WRAP=${NEWT_FLAG[WRAP]:-64}
readonly FLAG_PASSWORD=${NEWT_FLAG[PASSWORD]:-2048}

# ── parse command-line options ───────────────────────────────────────────────
_whiptail_mode=""
_whiptail_title=""
_whiptail_backtitle=""
_whiptail_clear=0
_whiptail_defaultno=0
_whiptail_default_item=""
_whiptail_fullbuttons=0
_whiptail_nocancel=0
_whiptail_noitem=0
_whiptail_notags=0
_whiptail_scrolltext=0
_whiptail_separate_output=0
_whiptail_topleft=0
_whiptail_outputfd=2
_whiptail_yes_button=""
_whiptail_no_button=""
_whiptail_ok_button=""
_whiptail_cancel_button=""
_whiptail_positional=()

_whiptail_usage() {
    cat >&2 <<'EOF'
Box options:
	--msgbox <text> <height> <width>
	--yesno  <text> <height> <width>
	--infobox <text> <height> <width>
	--inputbox <text> <height> <width> [init]
	--passwordbox <text> <height> <width> [init]
	--textbox <file> <height> <width>
	--menu <text> <height> <width> <listheight> [tag item] ...
	--checklist <text> <height> <width> <listheight> [tag item status]...
	--radiolist <text> <height> <width> <listheight> [tag item status]...
	--gauge <text> <height> <width> <percent>
Options: (depend on box-option)
	--clear				clear screen on exit
	--defaultno			default no button
	--default-item <string>		set default string
	--fb, --fullbuttons		use full buttons
	--nocancel			no cancel button
	--yes-button <text>		set text of yes button
	--no-button <text>		set text of no button
	--ok-button <text>		set text of ok button
	--cancel-button <text>		set text of cancel button
	--noitem			don't display items
	--notags			don't display tags
	--separate-output		output one line at a time
	--output-fd <fd>		output to fd, not stderr
	--title <title>			display title
	--backtitle <backtitle>		display backtitle
	--scrolltext			force vertical scrollbars
	--topleft			put window in top-left corner
	-h, --help			print this message
	-v, --version			print version information
EOF
    exit "${1:-1}"
}

_whiptail_version() {
    echo "whiptail.sh (newt_builtin) 0.1"
    exit 0
}

# Parse arguments.  Everything after "--" or after the box-option + text/h/w
# goes into positional.
_whiptail_parse() {
    while [[ $# -gt 0 ]]; do
        case "$1" in
            --title)       _whiptail_title="$2"; shift 2 ;;
            --backtitle)   _whiptail_backtitle="$2"; shift 2 ;;
            --clear)       _whiptail_clear=1; shift ;;
            --defaultno)   _whiptail_defaultno=1; shift ;;
            --default-item) _whiptail_default_item="$2"; shift 2 ;;
            --fb|--fullbuttons) _whiptail_fullbuttons=1; shift ;;
            --nocancel)    _whiptail_nocancel=1; shift ;;
            --noitem)      _whiptail_noitem=1; shift ;;
            --notags)      _whiptail_notags=1; shift ;;
            --scrolltext)  _whiptail_scrolltext=1; shift ;;
            --separate-output) _whiptail_separate_output=1; shift ;;
            --output-fd)   _whiptail_outputfd="$2"; shift 2 ;;
            --topleft)     _whiptail_topleft=1; shift ;;
            --yes-button)  _whiptail_yes_button="$2"; shift 2 ;;
            --no-button)   _whiptail_no_button="$2"; shift 2 ;;
            --ok-button)   _whiptail_ok_button="$2"; shift 2 ;;
            --cancel-button) _whiptail_cancel_button="$2"; shift 2 ;;
            -h|--help)     _whiptail_usage 0 ;;
            -v|--version)  _whiptail_version ;;
            --msgbox)      _whiptail_mode=msgbox; shift ;;
            --yesno)       _whiptail_mode=yesno; shift ;;
            --infobox)     _whiptail_mode=infobox; shift ;;
            --inputbox)    _whiptail_mode=inputbox; shift ;;
            --passwordbox) _whiptail_mode=passwordbox; shift ;;
            --textbox)     _whiptail_mode=textbox; shift ;;
            --menu)        _whiptail_mode=menu; shift ;;
            --checklist)   _whiptail_mode=checklist; shift ;;
            --radiolist)   _whiptail_mode=radiolist; shift ;;
            --gauge)       _whiptail_mode=gauge; shift ;;
            --)            shift; _whiptail_positional+=("$@"); break ;;
            -*)            echo >&2 "whiptail.sh: unknown option: $1"; _whiptail_usage 1 ;;
            *)             _whiptail_positional+=("$1"); shift ;;
        esac
    done
}

# ── helper: clean \n escape sequences into real newlines ─────────────────────
_whiptail_clean_newlines() {
    local text="$1"
    # Replace literal two-char sequence \n with a real newline
    printf '%s' "${text//\\n/$'\n'}"
}

# ── helper: compute window position ─────────────────────────────────────────
# Sets _wt_left, _wt_top based on screen size and requested dimensions.
_whiptail_window_pos() {
    local w=$1 h=$2
    newt GetScreenSize _wt_scr_cols _wt_scr_rows
    if (( _whiptail_topleft )); then
        _wt_left=1
        _wt_top=1
    else
        _wt_left=$(( (_wt_scr_cols - w) / 2 ))
        _wt_top=$(( (_wt_scr_rows - h) / 2 ))
    fi
}

# ── helper: make a button creator function ───────────────────────────────────
_whiptail_make_button() {
    if (( _whiptail_fullbuttons )); then
        newt -v "$1" Button "$2" "$3" "$4"
    else
        newt -v "$1" CompactButton "$2" "$3" "$4"
    fi
}

# ── helper: compute button height ───────────────────────────────────────────
_whiptail_button_height() {
    if (( _whiptail_fullbuttons )); then
        echo 3
    else
        echo 1
    fi
}

# ── auto-size support ────────────────────────────────────────────────────────
# If height or width is 0, compute reasonable defaults.
_whiptail_autosize() {
    local text="$1"
    local -n _h=$2 _w=$3

    newt GetScreenSize _wt_scr_cols _wt_scr_rows

    if (( _w == 0 )); then
        # Estimate width from text + border overhead
        local max_line_len=0
        local line
        while IFS= read -r line; do
            (( ${#line} > max_line_len )) && max_line_len=${#line}
        done <<< "$text"
        _w=$(( max_line_len + 6 ))
        # Also consider title width
        if [[ -n "$_whiptail_title" ]]; then
            local tw=$(( ${#_whiptail_title} + 4 ))
            (( tw > _w )) && _w=$tw
        fi
        (( _w < 20 )) && _w=20
        (( _w > _wt_scr_cols )) && _w=$_wt_scr_cols
    fi

    if (( _h == 0 )); then
        # Count lines of text, add border + button space
        local nlines=0
        local line
        while IFS= read -r line; do
            (( nlines++ ))
            # Account for wrapping
            if (( ${#line} > (_w - 4) && (_w - 4) > 0 )); then
                (( nlines += ${#line} / (_w - 4) ))
            fi
        done <<< "$text"
        local btnh
        btnh=$(_whiptail_button_height)
        _h=$(( nlines + btnh + 4 ))
        (( _h > _wt_scr_rows - 1 )) && _h=$(( _wt_scr_rows - 1 ))
    fi
}

# ── msgbox ───────────────────────────────────────────────────────────────────
_whiptail_msgbox() {
    local text="$1" height=$2 width=$3
    text=$(_whiptail_clean_newlines "$text")
    _whiptail_autosize "$text" height width

    local iw=$(( width - 2 )) ih=$(( height - 2 ))
    _whiptail_window_pos "$iw" "$ih"
    newt OpenWindow "$_wt_left" "$_wt_top" "$iw" "$ih" "$_whiptail_title"

    local btnh
    btnh=$(_whiptail_button_height)
    local tflags=$(( FLAG_WRAP ))
    (( _whiptail_scrolltext )) && tflags=$(( tflags | FLAG_SCROLL ))

    local text_h=$(( ih - 2 - btnh ))
    (( text_h < 1 )) && text_h=1
    newt -v _wt_tb Textbox 1 1 $(( iw - 2 )) "$text_h" "$tflags"
    newt TextboxSetText "$_wt_tb" "$text"

    local ok_text="${_whiptail_ok_button:-Ok}"
    local ok_left=$(( (iw - ${#ok_text} - 4) / 2 ))
    (( ok_left < 1 )) && ok_left=1
    _whiptail_make_button _wt_ok "$ok_left" $(( ih - btnh - 1 )) "$ok_text"

    newt -v _wt_form Form "" "" 0
    newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_ok"

    newt -v _wt_answer RunForm "$_wt_form"
    local rc=0
    [[ "$_wt_answer" == "(nil)" ]] && rc=255  # ESC pressed

    newt FormDestroy "$_wt_form"
    newt PopWindow
    return $rc
}

# ── infobox ──────────────────────────────────────────────────────────────────
_whiptail_infobox() {
    local text="$1" height=$2 width=$3
    text=$(_whiptail_clean_newlines "$text")
    _whiptail_autosize "$text" height width

    local iw=$(( width - 2 )) ih=$(( height - 2 ))
    _whiptail_window_pos "$iw" "$ih"
    newt OpenWindow "$_wt_left" "$_wt_top" "$iw" "$ih" "$_whiptail_title"

    local tflags=$(( FLAG_WRAP ))
    (( _whiptail_scrolltext )) && tflags=$(( tflags | FLAG_SCROLL ))

    newt -v _wt_tb Textbox 1 1 $(( iw - 2 )) $(( ih - 2 )) "$tflags"
    newt TextboxSetText "$_wt_tb" "$text"

    newt -v _wt_form Form "" "" 0
    newt FormAddComponents "$_wt_form" "$_wt_tb"

    newt DrawForm "$_wt_form"
    newt Refresh

    newt FormDestroy "$_wt_form"
    newt PopWindow
    return 0
}

# ── yesno ────────────────────────────────────────────────────────────────────
_whiptail_yesno() {
    local text="$1" height=$2 width=$3
    text=$(_whiptail_clean_newlines "$text")
    _whiptail_autosize "$text" height width

    local iw=$(( width - 2 )) ih=$(( height - 2 ))
    _whiptail_window_pos "$iw" "$ih"
    newt OpenWindow "$_wt_left" "$_wt_top" "$iw" "$ih" "$_whiptail_title"

    local btnh
    btnh=$(_whiptail_button_height)
    local tflags=$(( FLAG_WRAP ))
    (( _whiptail_scrolltext )) && tflags=$(( tflags | FLAG_SCROLL ))

    local text_h=$(( ih - 2 - btnh ))
    (( text_h < 1 )) && text_h=1
    newt -v _wt_tb Textbox 1 1 $(( iw - 2 )) "$text_h" "$tflags"
    newt TextboxSetText "$_wt_tb" "$text"

    local yes_text="${_whiptail_yes_button:-Yes}"
    local no_text="${_whiptail_no_button:-No}"
    local btn_row=$(( ih - btnh - 1 ))
    local yes_left=$(( (iw - ${#yes_text} - ${#no_text} - 12) / 3 ))
    (( yes_left < 1 )) && yes_left=1
    local no_left=$(( yes_left * 2 + ${#yes_text} + 5 ))

    _whiptail_make_button _wt_yes "$yes_left" "$btn_row" "$yes_text"
    _whiptail_make_button _wt_no  "$no_left"  "$btn_row" "$no_text"

    newt -v _wt_form Form "" "" 0
    newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_yes" "$_wt_no"

    if (( _whiptail_defaultno )); then
        newt FormSetCurrent "$_wt_form" "$_wt_no"
    fi

    newt -v _wt_answer RunForm "$_wt_form"
    local rc=0
    if [[ "$_wt_answer" == "$_wt_no" ]]; then
        rc=1
    elif [[ "$_wt_answer" == "(nil)" ]]; then
        rc=255
    fi

    newt FormDestroy "$_wt_form"
    newt PopWindow
    return $rc
}

# ── inputbox / passwordbox ───────────────────────────────────────────────────
_whiptail_inputbox() {
    local text="$1" height=$2 width=$3
    local init="${4:-}"
    local is_password="${5:-0}"
    text=$(_whiptail_clean_newlines "$text")
    _whiptail_autosize "$text" height width

    local iw=$(( width - 2 )) ih=$(( height - 2 ))
    _whiptail_window_pos "$iw" "$ih"
    newt OpenWindow "$_wt_left" "$_wt_top" "$iw" "$ih" "$_whiptail_title"

    local btnh
    btnh=$(_whiptail_button_height)
    local tflags=$(( FLAG_WRAP ))
    (( _whiptail_scrolltext )) && tflags=$(( tflags | FLAG_SCROLL ))

    local text_h=$(( ih - 3 - btnh ))
    (( text_h < 1 )) && text_h=1
    newt -v _wt_tb Textbox 1 1 $(( iw - 2 )) "$text_h" "$tflags"
    newt TextboxSetText "$_wt_tb" "$text"

    local entry_flags=$(( FLAG_SCROLL | FLAG_RETURNEXIT ))
    (( is_password )) && entry_flags=$(( entry_flags | FLAG_PASSWORD ))
    newt -v _wt_entry Entry 1 $(( text_h + 1 )) "$init" $(( iw - 2 )) "$entry_flags"

    local ok_text="${_whiptail_ok_button:-Ok}"
    local cancel_text="${_whiptail_cancel_button:-Cancel}"
    local btn_row=$(( ih - btnh - 1 ))

    if (( _whiptail_nocancel )); then
        local ok_left=$(( (iw - ${#ok_text} - 4) / 2 ))
        (( ok_left < 1 )) && ok_left=1
        _whiptail_make_button _wt_ok "$ok_left" "$btn_row" "$ok_text"
        newt -v _wt_form Form "" "" 0
        newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_entry" "$_wt_ok"
    else
        local ok_left=$(( (iw - ${#ok_text} - ${#cancel_text} - 14) / 3 ))
        (( ok_left < 1 )) && ok_left=1
        local cancel_left=$(( ok_left * 2 + ${#ok_text} + 5 ))
        _whiptail_make_button _wt_ok     "$ok_left"     "$btn_row" "$ok_text"
        _whiptail_make_button _wt_cancel "$cancel_left" "$btn_row" "$cancel_text"
        newt -v _wt_form Form "" "" 0
        newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_entry" "$_wt_ok" "$_wt_cancel"
    fi

    newt -v _wt_answer RunForm "$_wt_form"
    local rc=0
    if [[ "${_wt_cancel:-}" != "" && "$_wt_answer" == "$_wt_cancel" ]]; then
        rc=1
    elif [[ "$_wt_answer" == "(nil)" ]]; then
        rc=255
    else
        newt -v _wt_val EntryGetValue "$_wt_entry"
        printf '%s' "$_wt_val" >&"$_whiptail_outputfd"
    fi

    newt FormDestroy "$_wt_form"
    newt PopWindow
    return $rc
}

# ── textbox ──────────────────────────────────────────────────────────────────
_whiptail_textbox() {
    local file="$1" height=$2 width=$3

    if [[ ! -r "$file" ]]; then
        echo >&2 "whiptail.sh: cannot read file: $file"
        return 255
    fi
    local text
    text=$(<"$file")

    _whiptail_autosize "$text" height width

    local iw=$(( width - 2 )) ih=$(( height - 2 ))
    _whiptail_window_pos "$iw" "$ih"
    newt OpenWindow "$_wt_left" "$_wt_top" "$iw" "$ih" "$_whiptail_title"

    local btnh
    btnh=$(_whiptail_button_height)

    local text_h=$(( ih - 2 - btnh ))
    (( text_h < 1 )) && text_h=1
    local tflags=$(( FLAG_WRAP | FLAG_SCROLL ))
    newt -v _wt_tb Textbox 1 1 $(( iw - 2 )) "$text_h" "$tflags"
    newt TextboxSetText "$_wt_tb" "$text"

    local ok_text="${_whiptail_ok_button:-Ok}"
    local ok_left=$(( (iw - ${#ok_text} - 4) / 2 ))
    (( ok_left < 1 )) && ok_left=1
    _whiptail_make_button _wt_ok "$ok_left" $(( ih - btnh - 1 )) "$ok_text"

    newt -v _wt_form Form "" "" 0
    newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_ok"

    newt -v _wt_answer RunForm "$_wt_form"
    local rc=0
    [[ "$_wt_answer" == "(nil)" ]] && rc=255

    newt FormDestroy "$_wt_form"
    newt PopWindow
    return $rc
}

# ── menu ─────────────────────────────────────────────────────────────────────
_whiptail_menu() {
    local text="$1" height=$2 width=$3 list_height=$4
    shift 4
    text=$(_whiptail_clean_newlines "$text")

    # Remaining args: tag item tag item ...
    local -a tags=() items=()
    while [[ $# -ge 2 ]]; do
        tags+=("$1")
        items+=("$2")
        shift 2
    done

    _whiptail_autosize "$text" height width

    local iw=$(( width - 2 )) ih=$(( height - 2 ))
    _whiptail_window_pos "$iw" "$ih"
    newt OpenWindow "$_wt_left" "$_wt_top" "$iw" "$ih" "$_whiptail_title"

    local btnh
    btnh=$(_whiptail_button_height)
    local tflags=$(( FLAG_WRAP ))
    (( _whiptail_scrolltext )) && tflags=$(( tflags | FLAG_SCROLL ))

    local text_h=$(( ih - 3 - btnh - list_height ))
    (( text_h < 1 )) && text_h=1
    newt -v _wt_tb Textbox 1 1 $(( iw - 2 )) "$text_h" "$tflags"
    newt TextboxSetText "$_wt_tb" "$text"

    # Create listbox
    local lb_flags=$(( FLAG_RETURNEXIT ))
    local lb_h=$list_height
    if (( ${#tags[@]} > list_height )); then
        lb_flags=$(( lb_flags | FLAG_SCROLL ))
    fi
    local lb_top=$(( text_h + 1 ))
    newt -v _wt_lb Listbox 1 "$lb_top" "$lb_h" "$lb_flags"
    newt ListboxSetWidth "$_wt_lb" $(( iw - 2 ))

    local i max_tag_w=0 max_item_w=0
    for (( i=0; i<${#tags[@]}; i++ )); do
        (( ${#tags[i]} > max_tag_w )) && max_tag_w=${#tags[i]}
        (( ${#items[i]} > max_item_w )) && max_item_w=${#items[i]}
    done

    # Build display entries
    for (( i=0; i<${#tags[@]}; i++ )); do
        local display
        if (( _whiptail_notags )); then
            display="${items[i]}"
        elif (( _whiptail_noitem )); then
            display="${tags[i]}"
        else
            display=$(printf "%-${max_tag_w}s  %s" "${tags[i]}" "${items[i]}")
        fi
        newt ListboxAppendEntry "$_wt_lb" "$display" "$i"
    done

    # Set default selection
    if [[ -n "$_whiptail_default_item" ]]; then
        for (( i=0; i<${#tags[@]}; i++ )); do
            if [[ "${tags[i]}" == "$_whiptail_default_item" ]]; then
                newt ListboxSetCurrent "$_wt_lb" "$i"
                break
            fi
        done
    fi

    # Buttons
    local ok_text="${_whiptail_ok_button:-Ok}"
    local cancel_text="${_whiptail_cancel_button:-Cancel}"
    local btn_row=$(( ih - btnh - 1 ))

    if (( _whiptail_nocancel )); then
        _whiptail_make_button _wt_ok $(( (iw - ${#ok_text} - 4) / 2 )) "$btn_row" "$ok_text"
        newt -v _wt_form Form "" "" 0
        newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_lb" "$_wt_ok"
    else
        local ok_left=$(( (iw - ${#ok_text} - ${#cancel_text} - 14) / 3 ))
        (( ok_left < 1 )) && ok_left=1
        local cancel_left=$(( ok_left * 2 + ${#ok_text} + 5 ))
        _whiptail_make_button _wt_ok     "$ok_left"     "$btn_row" "$ok_text"
        _whiptail_make_button _wt_cancel "$cancel_left" "$btn_row" "$cancel_text"
        newt -v _wt_form Form "" "" 0
        newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_lb" "$_wt_ok" "$_wt_cancel"
    fi

    newt -v _wt_answer RunForm "$_wt_form"
    local rc=0
    if [[ "${_wt_cancel:-}" != "" && "$_wt_answer" == "$_wt_cancel" ]]; then
        rc=1
    elif [[ "$_wt_answer" == "(nil)" ]]; then
        rc=255
    else
        newt -v _wt_sel ListboxGetCurrent "$_wt_lb"
        local idx="$_wt_sel"
        printf '%s' "${tags[idx]}" >&"$_whiptail_outputfd"
    fi

    newt FormDestroy "$_wt_form"
    newt PopWindow
    return $rc
}

# ── checklist ────────────────────────────────────────────────────────────────
_whiptail_checklist() {
    local text="$1" height=$2 width=$3 list_height=$4
    shift 4
    text=$(_whiptail_clean_newlines "$text")

    # Remaining args: tag item status tag item status ...
    local -a tags=() items=() statuses=()
    while [[ $# -ge 3 ]]; do
        tags+=("$1")
        items+=("$2")
        statuses+=("$3")
        shift 3
    done

    _whiptail_autosize "$text" height width

    local iw=$(( width - 2 )) ih=$(( height - 2 ))
    _whiptail_window_pos "$iw" "$ih"
    newt OpenWindow "$_wt_left" "$_wt_top" "$iw" "$ih" "$_whiptail_title"

    local btnh
    btnh=$(_whiptail_button_height)
    local tflags=$(( FLAG_WRAP ))
    (( _whiptail_scrolltext )) && tflags=$(( tflags | FLAG_SCROLL ))

    local text_h=$(( ih - 3 - btnh - list_height ))
    (( text_h < 1 )) && text_h=1
    newt -v _wt_tb Textbox 1 1 $(( iw - 2 )) "$text_h" "$tflags"
    newt TextboxSetText "$_wt_tb" "$text"

    # Build checkbox labels and create checkbox components
    local i max_tag_w=0
    for (( i=0; i<${#tags[@]}; i++ )); do
        (( ${#tags[i]} > max_tag_w )) && max_tag_w=${#tags[i]}
    done

    # We need a scrollable subform for the checkboxes if they exceed list_height
    local cb_top=$(( text_h + 1 ))
    local -a cb_comps=()
    local -a cb_states=()

    # Create a vertical scrollbar if needed
    if (( ${#tags[@]} > list_height )); then
        newt -v _wt_sb VerticalScrollbar $(( iw - 3 )) "$cb_top" "$list_height" \
            "${NEWT_COLORSET[CHECKBOX]}" "${NEWT_COLORSET[ACTCHECKBOX]}"
        newt -v _wt_subform Form "$_wt_sb" "" 0
    else
        newt -v _wt_subform Form "" "" 0
    fi
    newt FormSetBackground "$_wt_subform" "${NEWT_COLORSET[CHECKBOX]}"

    for (( i=0; i<${#tags[@]}; i++ )); do
        local display
        if (( _whiptail_notags )); then
            display="${items[i]}"
        elif (( _whiptail_noitem )); then
            display="${tags[i]}"
        else
            display=$(printf "%-${max_tag_w}s  %s" "${tags[i]}" "${items[i]}")
        fi

        local default_val=" "
        case "${statuses[i],,}" in
            on|1|yes) default_val="*" ;;
        esac

        newt -v "_wt_cb_$i" Checkbox 2 $(( cb_top + i )) "$display" "$default_val"
        local ref="_wt_cb_$i"
        newt CheckboxSetFlags "${!ref}" "$FLAG_RETURNEXIT" 0
        cb_comps+=("${!ref}")
        newt FormAddComponent "$_wt_subform" "${!ref}"
    done

    newt FormSetHeight "$_wt_subform" "$list_height"
    newt FormSetWidth "$_wt_subform" $(( iw - 6 ))

    # Buttons
    local ok_text="${_whiptail_ok_button:-Ok}"
    local cancel_text="${_whiptail_cancel_button:-Cancel}"
    local btn_row=$(( ih - btnh - 1 ))

    newt -v _wt_form Form "" "" 0

    if (( ${#tags[@]} > list_height )); then
        newt FormAddComponent "$_wt_form" "$_wt_sb"
    fi

    newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_subform"

    if (( _whiptail_nocancel )); then
        _whiptail_make_button _wt_ok $(( (iw - ${#ok_text} - 4) / 2 )) "$btn_row" "$ok_text"
        newt FormAddComponent "$_wt_form" "$_wt_ok"
    else
        local ok_left=$(( (iw - ${#ok_text} - ${#cancel_text} - 14) / 3 ))
        (( ok_left < 1 )) && ok_left=1
        local cancel_left=$(( ok_left * 2 + ${#ok_text} + 5 ))
        _whiptail_make_button _wt_ok     "$ok_left"     "$btn_row" "$ok_text"
        _whiptail_make_button _wt_cancel "$cancel_left" "$btn_row" "$cancel_text"
        newt FormAddComponents "$_wt_form" "$_wt_ok" "$_wt_cancel"
    fi

    newt -v _wt_answer RunForm "$_wt_form"
    local rc=0
    if [[ "${_wt_cancel:-}" != "" && "$_wt_answer" == "$_wt_cancel" ]]; then
        rc=1
    elif [[ "$_wt_answer" == "(nil)" ]]; then
        rc=255
    else
        # Collect checked items
        local need_space=0
        for (( i=0; i<${#tags[@]}; i++ )); do
            local ref="_wt_cb_$i"
            newt -v _wt_cbval CheckboxGetValue "${!ref}"
            if [[ "$_wt_cbval" != " " ]]; then
                if (( _whiptail_separate_output )); then
                    printf '%s\n' "${tags[i]}" >&"$_whiptail_outputfd"
                else
                    (( need_space )) && printf ' ' >&"$_whiptail_outputfd"
                    printf '"%s"' "${tags[i]}" >&"$_whiptail_outputfd"
                    need_space=1
                fi
            fi
        done
    fi

    newt FormDestroy "$_wt_form"
    newt PopWindow
    return $rc
}

# ── radiolist ────────────────────────────────────────────────────────────────
_whiptail_radiolist() {
    local text="$1" height=$2 width=$3 list_height=$4
    shift 4
    text=$(_whiptail_clean_newlines "$text")

    # Remaining args: tag item status tag item status ...
    local -a tags=() items=() statuses=()
    while [[ $# -ge 3 ]]; do
        tags+=("$1")
        items+=("$2")
        statuses+=("$3")
        shift 3
    done

    _whiptail_autosize "$text" height width

    local iw=$(( width - 2 )) ih=$(( height - 2 ))
    _whiptail_window_pos "$iw" "$ih"
    newt OpenWindow "$_wt_left" "$_wt_top" "$iw" "$ih" "$_whiptail_title"

    local btnh
    btnh=$(_whiptail_button_height)
    local tflags=$(( FLAG_WRAP ))
    (( _whiptail_scrolltext )) && tflags=$(( tflags | FLAG_SCROLL ))

    local text_h=$(( ih - 3 - btnh - list_height ))
    (( text_h < 1 )) && text_h=1
    newt -v _wt_tb Textbox 1 1 $(( iw - 2 )) "$text_h" "$tflags"
    newt TextboxSetText "$_wt_tb" "$text"

    local i max_tag_w=0
    for (( i=0; i<${#tags[@]}; i++ )); do
        (( ${#tags[i]} > max_tag_w )) && max_tag_w=${#tags[i]}
    done

    # Create radiobuttons in a scrollable subform
    local rb_top=$(( text_h + 1 ))
    local -a rb_comps=()
    local prev_rb=""

    if (( ${#tags[@]} > list_height )); then
        newt -v _wt_sb VerticalScrollbar $(( iw - 3 )) "$rb_top" "$list_height" \
            "${NEWT_COLORSET[CHECKBOX]}" "${NEWT_COLORSET[ACTCHECKBOX]}"
        newt -v _wt_subform Form "$_wt_sb" "" 0
    else
        newt -v _wt_subform Form "" "" 0
    fi
    newt FormSetBackground "$_wt_subform" "${NEWT_COLORSET[CHECKBOX]}"

    for (( i=0; i<${#tags[@]}; i++ )); do
        local display
        if (( _whiptail_notags )); then
            display="${items[i]}"
        elif (( _whiptail_noitem )); then
            display="${tags[i]}"
        else
            display=$(printf "%-${max_tag_w}s  %s" "${tags[i]}" "${items[i]}")
        fi

        local is_default=0
        case "${statuses[i],,}" in
            on|1|yes) is_default=1 ;;
        esac

        if [[ -z "$prev_rb" ]]; then
            newt -v "_wt_rb_$i" Radiobutton 2 $(( rb_top + i )) "$display" "$is_default" ""
        else
            newt -v "_wt_rb_$i" Radiobutton 2 $(( rb_top + i )) "$display" "$is_default" "$prev_rb"
        fi
        local ref="_wt_rb_$i"
        prev_rb="${!ref}"
        newt CheckboxSetFlags "$prev_rb" "$FLAG_RETURNEXIT" 0
        rb_comps+=("$prev_rb")
        newt FormAddComponent "$_wt_subform" "$prev_rb"
    done

    newt FormSetHeight "$_wt_subform" "$list_height"
    newt FormSetWidth "$_wt_subform" $(( iw - 6 ))

    local ok_text="${_whiptail_ok_button:-Ok}"
    local cancel_text="${_whiptail_cancel_button:-Cancel}"
    local btn_row=$(( ih - btnh - 1 ))

    newt -v _wt_form Form "" "" 0

    if (( ${#tags[@]} > list_height )); then
        newt FormAddComponent "$_wt_form" "$_wt_sb"
    fi

    newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_subform"

    if (( _whiptail_nocancel )); then
        _whiptail_make_button _wt_ok $(( (iw - ${#ok_text} - 4) / 2 )) "$btn_row" "$ok_text"
        newt FormAddComponent "$_wt_form" "$_wt_ok"
    else
        local ok_left=$(( (iw - ${#ok_text} - ${#cancel_text} - 14) / 3 ))
        (( ok_left < 1 )) && ok_left=1
        local cancel_left=$(( ok_left * 2 + ${#ok_text} + 5 ))
        _whiptail_make_button _wt_ok     "$ok_left"     "$btn_row" "$ok_text"
        _whiptail_make_button _wt_cancel "$cancel_left" "$btn_row" "$cancel_text"
        newt FormAddComponents "$_wt_form" "$_wt_ok" "$_wt_cancel"
    fi

    newt -v _wt_answer RunForm "$_wt_form"
    local rc=0
    if [[ "${_wt_cancel:-}" != "" && "$_wt_answer" == "$_wt_cancel" ]]; then
        rc=1
    elif [[ "$_wt_answer" == "(nil)" ]]; then
        rc=255
    else
        # Find which radiobutton is selected
        newt -v _wt_sel RadioGetCurrent "${rb_comps[0]}"
        for (( i=0; i<${#rb_comps[@]}; i++ )); do
            if [[ "${rb_comps[i]}" == "$_wt_sel" ]]; then
                printf '%s' "${tags[i]}" >&"$_whiptail_outputfd"
                break
            fi
        done
    fi

    newt FormDestroy "$_wt_form"
    newt PopWindow
    return $rc
}

# ── gauge ────────────────────────────────────────────────────────────────────
_whiptail_gauge() {
    local text="$1" height=$2 width=$3 percent=$4
    text=$(_whiptail_clean_newlines "$text")
    _whiptail_autosize "$text" height width

    local iw=$(( width - 2 )) ih=$(( height - 2 ))
    _whiptail_window_pos "$iw" "$ih"
    newt OpenWindow "$_wt_left" "$_wt_top" "$iw" "$ih" "$_whiptail_title"

    local tflags=$(( FLAG_WRAP ))
    local text_h=$(( ih - 3 ))
    (( text_h < 1 )) && text_h=1
    newt -v _wt_tb Textbox 1 1 $(( iw - 2 )) "$text_h" "$tflags"
    newt TextboxSetText "$_wt_tb" "$text"

    newt -v _wt_scale Scale 2 $(( ih - 1 )) $(( iw - 4 )) 100
    newt ScaleSet "$_wt_scale" "$percent"

    newt -v _wt_form Form "" "" 0
    newt FormAddComponents "$_wt_form" "$_wt_tb" "$_wt_scale"

    newt DrawForm "$_wt_form"
    newt Refresh

    # Read percentage updates from stdin
    # Protocol: plain integer lines update the scale.
    # "XXX" starts a block: next line is percentage, subsequent lines until
    # the next "XXX" replace the text.
    local line
    while IFS= read -r line; do
        if [[ "$line" == "XXX" ]]; then
            # Read new percentage
            local new_pct new_text=""
            IFS= read -r new_pct || break
            # Read new text until next XXX or EOF
            while IFS= read -r line; do
                if [[ "$line" == "XXX" ]]; then
                    break
                fi
                if [[ -n "$new_text" ]]; then
                    new_text+=$'\n'
                fi
                new_text+="$line"
            done
            if [[ -n "$new_text" ]]; then
                new_text=$(_whiptail_clean_newlines "$new_text")
                newt TextboxSetText "$_wt_tb" "$new_text"
            fi
            if [[ "$new_pct" =~ ^[0-9]+$ ]]; then
                newt ScaleSet "$_wt_scale" "$new_pct"
            fi
            newt DrawForm "$_wt_form"
            newt Refresh
        elif [[ "$line" =~ ^[0-9]+$ ]]; then
            newt ScaleSet "$_wt_scale" "$line"
            newt DrawForm "$_wt_form"
            newt Refresh
        fi
    done

    newt FormDestroy "$_wt_form"
    newt PopWindow
    return 0
}

# ── main dispatch ────────────────────────────────────────────────────────────
_whiptail_parse "$@"

if [[ -z "$_whiptail_mode" ]]; then
    echo >&2 "whiptail.sh: no box option specified"
    _whiptail_usage 1
fi

# Extract text, height, width from positional args
if [[ ${#_whiptail_positional[@]} -lt 3 ]]; then
    echo >&2 "whiptail.sh: missing required arguments: text height width"
    _whiptail_usage 1
fi

_wt_text="${_whiptail_positional[0]}"
_wt_height="${_whiptail_positional[1]}"
_wt_width="${_whiptail_positional[2]}"

# Validate numeric height/width
if ! [[ "$_wt_height" =~ ^[0-9]+$ && "$_wt_width" =~ ^[0-9]+$ ]]; then
    echo >&2 "whiptail.sh: height and width must be numbers"
    exit 255
fi

# Initialize newt
newt Init
_wt_cleanup() {
    if (( _whiptail_clear )); then
        newt PopWindow 2>/dev/null || true
    fi
    newt Finished
}
trap '_wt_cleanup' EXIT
newt Cls

# Draw backtitle if requested
if [[ -n "$_whiptail_backtitle" ]]; then
    newt DrawRootText 0 0 "$_whiptail_backtitle"
fi

newt PushHelpLine ""

# Dispatch to the appropriate handler
_wt_rc=0
case "$_whiptail_mode" in
    msgbox)
        _whiptail_msgbox "$_wt_text" "$_wt_height" "$_wt_width" || _wt_rc=$?
        ;;
    infobox)
        _whiptail_infobox "$_wt_text" "$_wt_height" "$_wt_width" || _wt_rc=$?
        ;;
    yesno)
        _whiptail_yesno "$_wt_text" "$_wt_height" "$_wt_width" || _wt_rc=$?
        ;;
    inputbox)
        _whiptail_inputbox "$_wt_text" "$_wt_height" "$_wt_width" \
            "${_whiptail_positional[3]:-}" 0 || _wt_rc=$?
        ;;
    passwordbox)
        _whiptail_inputbox "$_wt_text" "$_wt_height" "$_wt_width" \
            "${_whiptail_positional[3]:-}" 1 || _wt_rc=$?
        ;;
    textbox)
        _whiptail_textbox "$_wt_text" "$_wt_height" "$_wt_width" || _wt_rc=$?
        ;;
    menu)
        if [[ ${#_whiptail_positional[@]} -lt 4 ]]; then
            echo >&2 "whiptail.sh: --menu requires list-height"
            exit 255
        fi
        _whiptail_menu "$_wt_text" "$_wt_height" "$_wt_width" \
            "${_whiptail_positional[3]}" "${_whiptail_positional[@]:4}" || _wt_rc=$?
        ;;
    checklist)
        if [[ ${#_whiptail_positional[@]} -lt 4 ]]; then
            echo >&2 "whiptail.sh: --checklist requires list-height"
            exit 255
        fi
        _whiptail_checklist "$_wt_text" "$_wt_height" "$_wt_width" \
            "${_whiptail_positional[3]}" "${_whiptail_positional[@]:4}" || _wt_rc=$?
        ;;
    radiolist)
        if [[ ${#_whiptail_positional[@]} -lt 4 ]]; then
            echo >&2 "whiptail.sh: --radiolist requires list-height"
            exit 255
        fi
        _whiptail_radiolist "$_wt_text" "$_wt_height" "$_wt_width" \
            "${_whiptail_positional[3]}" "${_whiptail_positional[@]:4}" || _wt_rc=$?
        ;;
    gauge)
        if [[ ${#_whiptail_positional[@]} -lt 4 ]]; then
            echo >&2 "whiptail.sh: --gauge requires percent"
            exit 255
        fi
        _whiptail_gauge "$_wt_text" "$_wt_height" "$_wt_width" \
            "${_whiptail_positional[3]}" || _wt_rc=$?
        ;;
    *)
        echo >&2 "whiptail.sh: unknown mode: $_whiptail_mode"
        exit 255
        ;;
esac

# Match whiptail exit codes: 0=OK/Yes, 1=Cancel/No, 255(-1)=ESC/error
exit "$_wt_rc"
