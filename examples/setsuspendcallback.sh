#!/bin/bash
# setsuspendcallback.sh — demonstrate newt SetSuspendCallback
#
# Registers a bash function as the suspend hook, then runs a form.
# Press Ctrl+Z while the form is open to trigger the callback; it saves
# the terminal state, prints a banner on the normal terminal, then resumes
# the newt UI.

# try possible locations
for lib in libs/ ../libs/ build/src/ ../build/src/; do
    if [[ -f $lib/newt.so ]]; then
        enable -f "$lib"/newt.so newt
        break
    fi
done

if ! type newt &>/dev/null; then
    echo >&2 "unable to load the newt builtin"
    exit 1
fi

# Counter so we can show how many times the callback has fired.
_suspend_count=0

# The suspend callback:
#  1. Call newt Suspend  — hands the terminal back to the normal shell.
#  2. Do whatever you like (print a message, run a shell command, etc.).
#  3. Call newt Resume   — restores the newt UI.
on_suspend() {
    newt Suspend
    (( _suspend_count++ ))
    echo ""
    echo "┌─────────────────────────────────────────────────┐"
    echo "│  Suspend callback fired (count: $_suspend_count)          │"
    echo "│  You could run a shell command here.             │"
    echo "│  Press <Enter> to return to the newt form...     │"
    echo "└─────────────────────────────────────────────────┘"
    read -r _ignored
    newt Resume
}

newt Init
trap 'newt Finished' EXIT
newt Cls

# Register the bash function as the suspend hook.
newt SetSuspendCallback on_suspend

newt DrawRootText 2 0 "Press Ctrl+Z to trigger the suspend callback"
newt OpenWindow 5 2 60 12 "SetSuspendCallback demo"

newt -v info  Label  3 1  "This form is waiting for input."
newt -v info2 Label  3 2  "Press  Ctrl+Z  to fire the suspend callback."
newt -v info3 Label  3 3  "Press  Enter / Tab  to return from the callback."
newt -v info4 Label  3 5  "Press  Enter  (on the OK button) to finish."
newt -v btn   Button 3 7  "OK"

newt -v f Form NULL NULL 0
newt FormAddComponents "$f" "$info" "$info2" "$info3" "$info4" "$btn"

newt RunForm "$f"
newt FormDestroy "$f"
newt PopWindow

newt DrawRootText 2 0 "Suspend callback was triggered $_suspend_count time(s).   "
newt Refresh
sleep 2
