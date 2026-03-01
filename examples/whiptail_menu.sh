#!/bin/bash
# whiptail_menu.sh — demonstrate --menu and --radiolist dialogs
#
# Usage:
#   bash examples/whiptail_menu.sh             # use whiptail.sh (builtin)
#   bash examples/whiptail_menu.sh --native     # use system whiptail

# ── select backend ───────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

if [[ "${1:-}" == "--native" ]]; then
    WT=whiptail
else
    WT="$SCRIPT_DIR/whiptail.sh"
fi

# ── menu ─────────────────────────────────────────────────────────────────────
# whiptail writes the selected tag to stderr (fd 2).
# We use the fd-swap trick: 3>&1 1>&2 2>&3 to capture it.
exec 3>&1
CHOICE=$("$WT" --title "Main Menu" \
               --menu "Choose an option:" 16 50 6 \
               "files"    "Browse files" \
               "network"  "Network settings" \
               "users"    "User management" \
               "packages" "Install packages" \
               "logs"     "View system logs" \
               "quit"     "Exit this demo" \
         2>&1 1>&3) || true
exec 3>&-

echo "Menu selection: ${CHOICE:-<cancelled>}"

# ── menu with --default-item ─────────────────────────────────────────────────
exec 3>&1
CHOICE=$("$WT" --title "Pick a Colour" \
               --default-item "green" \
               --menu "Your favourite colour:" 12 40 4 \
               "red"    "Red" \
               "green"  "Green" \
               "blue"   "Blue" \
               "yellow" "Yellow" \
         2>&1 1>&3) || true
exec 3>&-

echo "Colour: ${CHOICE:-<cancelled>}"

# ── radiolist ────────────────────────────────────────────────────────────────
exec 3>&1
SHELL_CHOICE=$("$WT" --title "Default Shell" \
                     --radiolist "Select your preferred shell:" 14 50 5 \
                     "bash"  "Bourne Again Shell" ON \
                     "zsh"   "Z Shell"            OFF \
                     "fish"  "Friendly Shell"     OFF \
                     "dash"  "Debian Almquist"    OFF \
                     "ksh"   "Korn Shell"         OFF \
               2>&1 1>&3) || true
exec 3>&-

echo "Shell: ${SHELL_CHOICE:-<cancelled>}"
