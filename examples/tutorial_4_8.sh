#!/bin/bash
# Tutorial section 4.8 – Checkboxes and Radio Buttons
#
# Port of the C example from the newt tutorial.
# Demonstrates: Checkbox, CheckboxGetValue, Radiobutton, RadioGetCurrent.
#
# Usage: bash tutorial_4_8.sh

# ── locate and load the newt builtin ─────────────────────────────────────────
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

if newt Init; then
    trap 'newt Finished' EXIT
    newt Cls

    newt OpenWindow 10 5 40 11 "Checkboxes and Radio buttons"

    # Checkbox: left top text [defValue [seq]]
    # defValue ' ' = unchecked; seq " *X" cycles through three states
    newt -v checkbox Checkbox 1 1 "A checkbox" ' ' " *X"

    # Radio buttons: left top text isDefault prevButton
    # Pass "" (empty string) as prevButton for the first button to start a new group.
    newt -v rb0 Radiobutton 1 3 "Choice 1" 1 ""
    newt -v rb1 Radiobutton 1 4 "Choice 2" 0 "$rb0"
    newt -v rb2 Radiobutton 1 5 "Choice 3" 0 "$rb1"

    newt -v button Button 1 7 "Ok"

    newt -v form Form
    newt FormAddComponents "$form" "$checkbox" "$rb0" "$rb1" "$rb2" "$button"

    newt RunForm "$form"

    # Read values before destroying the form
    newt -v cbValue CheckboxGetValue "$checkbox"
    newt -v current RadioGetCurrent  "$rb0"

    newt FormDestroy "$form"
fi

# Print results after newtFinished (called by the trap above)
for i in 0 1 2; do
    rb_var="rb${i}"
    if [[ "$current" == "${!rb_var}" ]]; then
        echo "radio button picked: $i"
        break
    fi
done
echo "checkbox value: '$cbValue'"
