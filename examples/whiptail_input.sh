#!/bin/bash
# whiptail_input.sh — demonstrate --inputbox, --passwordbox, and --textbox
#
# Usage:
#   bash examples/whiptail_input.sh             # use whiptail.sh (builtin)
#   bash examples/whiptail_input.sh --native     # use system whiptail

# ── select backend ───────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

if [[ "${1:-}" == "--native" ]]; then
    WT=whiptail
else
    WT="$SCRIPT_DIR/whiptail.sh"
fi

# ── inputbox ─────────────────────────────────────────────────────────────────
exec 3>&1
NAME=$("$WT" --title "User Info" \
             --inputbox "What is your name?" 8 40 "World" \
       2>&1 1>&3) || true
exec 3>&-

if [[ -n "$NAME" ]]; then
    echo "Hello, $NAME!"
else
    echo "No name entered."
fi

# ── passwordbox ──────────────────────────────────────────────────────────────
exec 3>&1
PASS=$("$WT" --title "Authentication" \
             --passwordbox "Enter your password:" 8 45 \
       2>&1 1>&3) || true
exec 3>&-

if [[ -n "$PASS" ]]; then
    echo "Password accepted (${#PASS} characters)."
else
    echo "No password entered."
fi

# ── inputbox with --nocancel ─────────────────────────────────────────────────
exec 3>&1
HOSTNAME=$("$WT" --title "Setup" \
                 --nocancel \
                 --inputbox "Enter the hostname:" 8 45 "myhost" \
           2>&1 1>&3) || true
exec 3>&-

echo "Hostname: ${HOSTNAME:-<empty>}"

# ── textbox (display a file) ─────────────────────────────────────────────────
# Create a temporary file to display
TMPFILE=$(mktemp)
trap 'rm -f "$TMPFILE"' EXIT

cat > "$TMPFILE" <<'HEREDOC'
whiptail.sh — Example Text Viewer
==================================

This text is displayed in a scrollable textbox.
You can navigate with UP/DOWN, PgUp/PgDn, and Home/End.

Features demonstrated:
  • --textbox reads a file and displays it
  • The content is scrollable when it exceeds the window
  • Press OK or Enter to dismiss

Lorem ipsum dolor sit amet, consectetur adipiscing elit.
Sed do eiusmod tempor incididunt ut labore et dolore magna
aliqua. Ut enim ad minim veniam, quis nostrud exercitation
ullamco laboris nisi ut aliquip ex ea commodo consequat.

Duis aute irure dolor in reprehenderit in voluptate velit
esse cillum dolore eu fugiat nulla pariatur.

End of file.
HEREDOC

"$WT" --title "README" \
      --scrolltext \
      --textbox "$TMPFILE" 15 60

echo "Textbox dismissed."
