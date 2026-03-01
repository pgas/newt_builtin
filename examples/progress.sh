#!/bin/bash
# progress.sh — animated progress bar driven by a background job.
#
# Demonstrates NEWT_FD[READ] and FormWatchFd:
#   A coproc writes progress values (0–100) to a pipe every 50 ms; the form
#   wakes on each write (reason=FDREADY) and advances a Scale widget.
#
# Key APIs used:
#   newt Scale left top width fullValue      – create the scale bar
#   newt ScaleSet co amount                  – advance the bar
#   newt ScaleSetColors co empty full        – colour the scale
#   newt FormWatchFd form fd ${NEWT_FD[READ]} – register the pipe watch
#   newt FormRun form REASON VALUE           – event loop (exits on FDREADY/COMPONENT)

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

# ── background worker coproc ──────────────────────────────────────────────────
# Writes integers 0, 2, 4, … 100 to its stdout (one per line), then exits.
# bash wires WORKER[0] = parent's read end, WORKER[1] = parent's write end of
# the coproc's stdin pipe (unused — close it immediately).
coproc WORKER {
    for pct in $(seq 0 2 100); do
        printf '%d\n' "$pct"
        sleep 0.05
    done
}
exec {WORKER[1]}>&-   # close unused stdin write-end in parent

# ── TUI ───────────────────────────────────────────────────────────────────────
newt Init
trap 'newt Finished; kill "${WORKER_PID:-}" 2>/dev/null; wait "${WORKER_PID:-}" 2>/dev/null' EXIT

newt Cls
newt GetScreenSize COLS ROWS

win_w=52
win_h=9
win_l=$(( (COLS - win_w) / 2 ))
win_t=$(( (ROWS - win_h) / 2 ))

newt OpenWindow "$win_l" "$win_t" "$win_w" "$win_h" "Progress"

# Status label (updated on each tick).
newt -v lbl Label 3 1 "Running background task…          "

# Scale widget: left=3 top=3 width=46 fullValue=100
newt -v scale Scale 3 3 46 100
newt ScaleSetColors "$scale" \
    "${NEWT_COLORSET[EMPTYSCALE]}" "${NEWT_COLORSET[FULLSCALE]}"

newt -v btn Button 3 5 "Cancel"

newt -v f Form "" "" 0
newt FormAddComponents "$f" "$lbl" "$scale" "$btn"

# Watch the coproc's stdout pipe for readability.
newt FormWatchFd "$f" "${WORKER[0]}" "${NEWT_FD[READ]}"

# ── event loop ────────────────────────────────────────────────────────────────
pct=0
cancelled=0

while true; do
    newt FormRun "$f" REASON VALUE

    case "$REASON" in
        FDREADY)
            # A value is ready on the pipe — read it.
            if IFS= read -r -u "${WORKER[0]}" pct 2>/dev/null; then
                newt ScaleSet       "$scale" "$pct"
                newt LabelSetText   "$lbl"   "Running background task…  ${pct}%"
                newt DrawForm "$f"
                newt Refresh
                (( pct >= 100 )) && break
            else
                # Pipe closed (worker done or killed).
                break
            fi
            ;;
        COMPONENT)
            # Cancel button pressed.
            cancelled=1
            break
            ;;
    esac
done

# Wait for the coproc to finish.
wait "${WORKER_PID:-}" 2>/dev/null

newt FormDestroy "$f"
newt PopWindow
newt Finished

# ── summary ───────────────────────────────────────────────────────────────────
if (( cancelled )); then
    printf 'Cancelled at %d%%.\n' "$pct"
elif (( pct >= 100 )); then
    printf 'Done! (100%%)\n'
else
    printf 'Stopped at %d%%.\n' "$pct"
fi
