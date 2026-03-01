#!/bin/bash
# whiptail_checklist.sh — demonstrate --checklist dialog
#
# Usage:
#   bash examples/whiptail_checklist.sh             # use whiptail.sh (builtin)
#   bash examples/whiptail_checklist.sh --native     # use system whiptail

# ── select backend ───────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

if [[ "${1:-}" == "--native" ]]; then
    WT=whiptail
else
    WT="$SCRIPT_DIR/whiptail.sh"
fi

# ── checklist (quoted output, default) ───────────────────────────────────────
exec 3>&1
PACKAGES=$("$WT" --title "Package Selection" \
                 --checklist "Select packages to install:" 16 55 7 \
                 "vim"     "Text editor"         ON \
                 "git"     "Version control"     ON \
                 "tmux"    "Terminal multiplexer" OFF \
                 "htop"    "Process viewer"       OFF \
                 "curl"    "URL transfer tool"    ON \
                 "jq"      "JSON processor"       OFF \
                 "ripgrep" "Fast grep"            OFF \
           2>&1 1>&3) || true
exec 3>&-

echo "Selected (quoted): $PACKAGES"

# ── checklist with --separate-output ─────────────────────────────────────────
exec 3>&1
FEATURES=$("$WT" --title "Feature Flags" \
                 --separate-output \
                 --checklist "Enable features:" 12 45 4 \
                 "debug"   "Debug mode"     OFF \
                 "verbose" "Verbose output" ON \
                 "color"   "Coloured output" ON \
                 "strict"  "Strict mode"    OFF \
           2>&1 1>&3) || true
exec 3>&-

echo "Enabled features (one per line):"
while IFS= read -r feat; do
    [[ -n "$feat" ]] && echo "  - $feat"
done <<< "$FEATURES"
