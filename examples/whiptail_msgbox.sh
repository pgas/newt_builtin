#!/bin/bash
# whiptail_msgbox.sh — demonstrate --msgbox and --yesno dialogs
#
# Usage:
#   bash examples/whiptail_msgbox.sh           # use whiptail.sh (builtin)
#   bash examples/whiptail_msgbox.sh --native   # use system whiptail

# ── select backend ───────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

if [[ "${1:-}" == "--native" ]]; then
    WT=whiptail
else
    WT="$SCRIPT_DIR/whiptail.sh"
fi

# ── msgbox ───────────────────────────────────────────────────────────────────
"$WT" --title "Welcome" \
      --msgbox "This is a simple message box.\n\nPress OK to continue." \
      10 50

# ── yesno ────────────────────────────────────────────────────────────────────
if "$WT" --title "Question" \
         --yesno "Do you like using the terminal?" \
         8 40; then
    echo "You answered Yes!"
else
    rc=$?
    if (( rc == 255 )); then
        echo "You pressed ESC."
    else
        echo "You answered No."
    fi
fi

# ── yesno with --defaultno ───────────────────────────────────────────────────
if "$WT" --title "Danger Zone" \
         --defaultno \
         --yes-button "Proceed" \
         --no-button "Abort" \
         --yesno "This action is destructive.\nAre you sure?" \
         9 45; then
    echo "User chose to proceed."
else
    echo "User chose to abort (safe choice)."
fi
