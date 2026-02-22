#!/bin/bash
# logtail.sh — live log viewer driven by FormWatchFd.
#
# Demonstrates NEWT_FD[READ] with a scrollable Textbox:
#   A coproc generates timestamped log lines; the form wakes on each new line
#   (reason=FDREADY), appends it to an in-memory buffer, and updates the
#   Textbox widget with the latest window of lines.
#
# Key APIs used:
#   newt Textbox left top width height ${NEWT_FLAG[SCROLL]}  – scrollable box
#   newt TextboxSetText co text                              – replace content
#   newt TextboxSetColors co normal active                   – custom colours
#   newt FormWatchFd form fd ${NEWT_FD[READ]}                – register watch
#   newt FormRun form REASON VALUE                           – event loop

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

# ── background log generator coproc ──────────────────────────────────────────
# Emits one timestamped line roughly every 700 ms.
coproc LOGGER {
    declare -a levels=(INFO INFO INFO WARN ERROR)
    declare -a actions=(
        "cache invalidated"       "heartbeat OK"
        "connection established"  "config reloaded"
        "disk flush completed"    "index rebuilt"
        "retry attempt"           "snapshot written"
    )
    n=1
    while true; do
        level="${levels[RANDOM % ${#levels[@]}]}"
        action="${actions[RANDOM % ${#actions[@]}]}"
        printf '%s  [%-5s]  job #%04d: %s\n' \
            "$(date '+%H:%M:%S')" "$level" "$n" "$action"
        (( n++ ))
        sleep 0.7
    done
}
exec {LOGGER[1]}>&-   # close unused stdin write-end in parent

# ── TUI ───────────────────────────────────────────────────────────────────────
newt Init
trap 'newt Finished; kill "${LOGGER_PID:-}" 2>/dev/null; wait "${LOGGER_PID:-}" 2>/dev/null' EXIT

newt Cls
newt GetScreenSize COLS ROWS

win_w=$(( COLS - 6 ))
win_h=$(( ROWS - 4 ))
win_l=3
win_t=2

# Textbox height = window interior minus borders minus button row minus gaps.
tb_h=$(( win_h - 5 ))
tb_w=$(( win_w - 4 ))

newt OpenWindow "$win_l" "$win_t" "$win_w" "$win_h" "Live Log"

# Scrollable textbox — NEWT_FLAG[SCROLL] enables the scroll bar.
newt -v tb Textbox 2 1 "$tb_w" "$tb_h" "${NEWT_FLAG[SCROLL]}"

# Colour the textbox: normal = TEXTBOX colorset, active = ACTTEXTBOX.
newt TextboxSetColors "$tb" \
    "${NEWT_COLORSET[TEXTBOX]}" "${NEWT_COLORSET[ACTTEXTBOX]}"

newt -v btn Button 2 $(( tb_h + 2 )) "Close"

newt -v f Form NULL NULL 0
newt FormAddComponents "$f" "$tb" "$btn"

# Watch the coproc pipe for incoming log lines.
newt FormWatchFd "$f" "${LOGGER[0]}" "${NEWT_FD[READ]}"

# ── event loop ────────────────────────────────────────────────────────────────
declare -a log_lines=()
max_lines=$(( tb_h - 1 ))   # keep at most this many lines in the textbox

while true; do
    newt FormRun "$f" REASON _

    case "$REASON" in
        FDREADY)
            if IFS= read -r -u "${LOGGER[0]}" line 2>/dev/null; then
                log_lines+=( "$line" )
                # Trim the oldest lines so the buffer stays within max_lines.
                while (( ${#log_lines[@]} > max_lines )); do
                    log_lines=( "${log_lines[@]:1}" )
                done
                newt TextboxSetText "$tb" "$(printf '%s\n' "${log_lines[@]}")"
                newt DrawForm "$f"
                newt Refresh
            else
                # Logger exited / pipe closed.
                break
            fi
            ;;
        COMPONENT)
            # Close button pressed.
            break
            ;;
    esac
done

kill "${LOGGER_PID:-}" 2>/dev/null
wait "${LOGGER_PID:-}" 2>/dev/null
newt FormDestroy "$f"
newt PopWindow
newt Finished

printf 'Log session ended. %d lines received.\n' "${#log_lines[@]}"
