#!/bin/bash
# whiptail_gauge.sh — demonstrate --gauge (progress bar) dialog
#
# Usage:
#   bash examples/whiptail_gauge.sh             # use whiptail.sh (builtin)
#   bash examples/whiptail_gauge.sh --native     # use system whiptail

# ── select backend ───────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

if [[ "${1:-}" == "--native" ]]; then
    WT=whiptail
else
    WT="$SCRIPT_DIR/whiptail.sh"
fi

# ── simple gauge: percentage-only updates ────────────────────────────────────
echo "=== Simple progress bar ==="
(
    for pct in 0 10 25 40 55 70 85 100; do
        echo "$pct"
        sleep 0.3
    done
) | "$WT" --title "Downloading" \
          --gauge "Downloading files..." 7 50 0

echo "Download complete!"

# ── gauge with text updates (XXX protocol) ───────────────────────────────────
echo "=== Progress bar with changing text ==="
(
    echo "10"
    sleep 0.5

    # Update both percentage and text
    echo "XXX"
    echo "30"
    echo "Copying configuration files..."
    echo "XXX"
    sleep 0.5

    echo "XXX"
    echo "50"
    echo "Installing dependencies...\n\nThis may take a moment."
    echo "XXX"
    sleep 0.5

    echo "XXX"
    echo "75"
    echo "Running post-install scripts..."
    echo "XXX"
    sleep 0.5

    echo "XXX"
    echo "100"
    echo "Installation complete!"
    echo "XXX"
    sleep 0.3
) | "$WT" --title "Setup Wizard" \
          --gauge "Preparing installation..." 8 55 0

echo "Setup finished!"
