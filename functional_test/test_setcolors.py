"""Functional tests for ``newt SetColors``.

SetColors replaces the entire active colour palette.  These tests verify that:
 1. Calling SetColors with the default-palette values does not crash and still
    allows normal rendering (text appears on screen).
 2. Calling SetColors with a custom palette still renders text correctly.
 3. Calling SetColors with too few arguments fails without crashing bash.
"""

import pexpect
from conftest import render, screen_rows, screen_text

# The 44 colour strings that match newtDefaultColorPalette, in struct-field order:
#   rootFg/Bg  borderFg/Bg  windowFg/Bg  shadowFg/Bg  titleFg/Bg
#   buttonFg/Bg  actButtonFg/Bg  checkboxFg/Bg  actCheckboxFg/Bg
#   entryFg/Bg  labelFg/Bg  listboxFg/Bg  actListboxFg/Bg
#   textboxFg/Bg  actTextboxFg/Bg  helpLineFg/Bg  rootTextFg/Bg
#   emptyScale  fullScale
#   disabledEntryFg/Bg  compactButtonFg/Bg  actSelListboxFg/Bg  selListboxFg/Bg
_DEFAULT_44 = (
    b"white blue "          # root
    b"black blue "          # border
    b"black white "         # window
    b"white black "         # shadow
    b"red white "           # title
    b"white red "           # button
    b"white red "           # actButton
    b"white black "         # checkbox
    b"white black "         # actCheckbox
    b"black white "         # entry
    b"blue white "          # label
    b"black white "         # listbox
    b"white blue "          # actListbox
    b"black white "         # textbox
    b"white blue "          # actTextbox
    b"black white "         # helpLine
    b"black white "         # rootText
    b"white "               # emptyScale
    b"red "                 # fullScale
    b"black white "         # disabledEntry
    b"white black "         # compactButton
    b"white blue "          # actSelListbox
    b"black white"          # selListbox
).strip()

assert len(_DEFAULT_44.split()) == 44, (
    f"Palette must have exactly 44 tokens, got {len(_DEFAULT_44.split())}"
)


def test_setcolors_default_palette_allows_rendering(bash_newt):
    """SetColors with the default palette should not crash; text must appear."""
    bash_newt.sendline(
        b"newt Init && "
        b"newt SetColors " + _DEFAULT_44 + b" && "
        b"newt Cls && "
        b'newt DrawRootText 0 0 "colors ok" && '
        b"newt Refresh && "
        b"newt WaitForKey && "
        b"newt Finished"
    )

    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)

    assert any("colors ok" in row for row in rows), (
        "'colors ok' not found on screen after SetColors.\n" + screen_text(screen)
    )
    bash_newt.send(b"\n")


def test_setcolors_custom_palette_allows_rendering(bash_newt):
    """SetColors with a fully custom palette: DrawRootText still renders."""
    # 22 pairs of green/black = 44 tokens.
    palette_44 = b" ".join([b"green", b"black"] * 22)
    assert len(palette_44.split()) == 44

    bash_newt.sendline(
        b"newt Init && "
        b"newt SetColors " + palette_44 + b" && "
        b"newt Cls && "
        b'newt DrawRootText 0 0 "custom colors" && '
        b"newt Refresh && "
        b"newt WaitForKey && "
        b"newt Finished"
    )

    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)

    assert any("custom colors" in row for row in rows), (
        "'custom colors' not found on screen.\n" + screen_text(screen)
    )
    bash_newt.send(b"\n")


def test_setcolors_too_few_args_returns_failure(bash_newt):
    """SetColors with fewer than 44 arguments must exit with non-zero status."""
    # Supply only 10 colour strings — far too few.
    bash_newt.sendline(
        b"newt Init; "
        b"newt SetColors white blue black blue black white white black red white; "
        b'echo "exit:$?"'
    )

    try:
        bash_newt.expect(b"exit:", timeout=3)
        bash_newt.expect(rb"\d+", timeout=1)
        code = int(bash_newt.match.group(0))
    except pexpect.TIMEOUT:
        code = -1

    assert code != 0, (
        "SetColors with too few args should return non-zero, got: " + str(code)
    )
    bash_newt.sendline(b"newt Finished")


# ─── SetColor ─────────────────────────────────────────────────────────────────

def test_setcolor_single_pair(bash_newt):
    """SetColor should update a single colorset without crashing."""
    # Use colorset 1 (NEWT_COLORSET_BORDER) with valid color names.
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt SetColor 1 white blue && '
        b'newt OpenWindow 5 3 50 8 "SetColor" && '
        b'newt -v lbl Label 3 2 "colour-ok" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$lbl" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("colour-ok" in r for r in rows), \
        f"Rendering after SetColor crashed or label not visible.\n{full}"

    bash_newt.send(b"\n")
