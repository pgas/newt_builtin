#!/bin/bash
# Tutorial section 4.10 – Textboxes
#
# Port of the C textbox/reflowed-text example from the newt tutorial.
# Demonstrates: TextboxReflowed, TextboxGetNumLines.
#
# Usage: bash tutorial_4_10.sh

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

# The $'...' quoting lets us embed real newlines via \n.
message=$'This is a pretty long message. It will be displayed \
in a newt textbox, and illustrates how to construct \
a textbox from arbitrary text which may not have \
very good line breaks.\n\nNotice how literal newline characters are \
respected, and may be used to force line breaks and blank lines.'

if newt Init; then
    trap 'newt Finished' EXIT
    newt Cls

    # Create the reflowed textbox first so we can measure its height.
    # TextboxReflowed: left top text width flexDown flexUp flags
    newt -v text TextboxReflowed 1 1 "$message" 30 5 5 0
    newt -v numlines TextboxGetNumLines "$text"

    newt -v button Button 12 $(( numlines + 2 )) "Ok"

    # Open the window sized to fit – must be done before running the form.
    newt OpenWindow 10 5 37 $(( numlines + 7 )) "Textboxes"

    newt -v form Form
    newt FormAddComponents "$form" "$text" "$button"

    newt RunForm "$form"
    newt FormDestroy "$form"
fi
