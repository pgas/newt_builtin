#!/bin/bash
# setcolors.sh — demonstrate newt SetColors and SetColor
#
# Shows the UI once with the default colour palette, then again after
# applying a fully custom palette (green-on-black, "retro terminal" look),
# so the colour change is clearly visible.

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

newt Init
trap 'newt Finished' EXIT
newt Cls

# ── Phase 1: default colours ──────────────────────────────────────────────────
newt DrawRootText 2 0 "Phase 1: default colours (press any key)"
newt OpenWindow 10 3 50 10 "Default Palette"

newt -v lbl  Label   5 1 "This uses the built-in default palette."
newt -v btn  Button  5 3 "Continue"
newt -v f    Form    NULL NULL 0
newt FormAddComponents "$f" "$lbl" "$btn"
newt RunForm "$f"
newt FormDestroy "$f"
newt PopWindow

# ── Phase 2: apply a fully custom palette (RetroTerm) ────────────────────────
# SetColors expects exactly 44 colour-name or RGB strings, one per
# struct newtColors field, in declaration order:
#   rootFg/Bg  borderFg/Bg  windowFg/Bg  shadowFg/Bg  titleFg/Bg
#   buttonFg/Bg  actButtonFg/Bg  checkboxFg/Bg  actCheckboxFg/Bg
#   entryFg/Bg  labelFg/Bg  listboxFg/Bg  actListboxFg/Bg
#   textboxFg/Bg  actTextboxFg/Bg  helpLineFg/Bg  rootTextFg/Bg
#   emptyScale  fullScale
#   disabledEntryFg/Bg  compactButtonFg/Bg  actSelListboxFg/Bg  selListboxFg/Bg
newt SetColors \
    green  black \
    green  black \
    green  black \
    black  green \
    brightgreen black \
    black  green \
    brightgreen black \
    green  black \
    brightgreen black \
    green  black \
    green  black \
    green  black \
    brightgreen black \
    green  black \
    brightgreen black \
    green  black \
    green  black \
    black \
    green \
    green  black \
    black  green \
    brightgreen black \
    green  black

newt Cls
newt DrawRootText 2 0 "Phase 2: custom 'RetroTerm' palette (press any key)"
newt OpenWindow 10 3 50 10 "Custom Palette"

newt -v lbl2 Label   5 1 "Colours applied via SetColors!"
newt -v btn2 Button  5 3 "Got it"
newt -v f2   Form    NULL NULL 0
newt FormAddComponents "$f2" "$lbl2" "$btn2"
newt RunForm "$f2"
newt FormDestroy "$f2"
newt PopWindow

# ── Phase 3: use SetColor to tweak a single colorset ─────────────────────────
# Reset root colours only (NEWT_COLORSET[ROOT]).
newt SetColor "${NEWT_COLORSET[ROOT]}" white blue
newt Cls
newt DrawRootText 2 0 "Phase 3: root colorset patched back to white-on-blue"
newt Refresh
sleep 2
