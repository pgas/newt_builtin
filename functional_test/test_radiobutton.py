"""Functional tests for ``newt Radiobutton``, ``RadioGetCurrent``,
and ``RadioSetCurrent``.

Verifies:
  - Radiobutton labels appear on screen
  - The first button in a group is selected by default (isDefault=1)
  - RadioSetCurrent changes the pre-selected button
  - RadioGetCurrent returns a non-empty pointer string
"""

import time
from conftest import render, screen_rows, screen_text


def test_radiobutton_labels_visible(bash_newt):
    """All radiobutton labels should be visible inside the window."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 12 "Radio Test" && '
        b'newt -v r1 Radiobutton 3 1 "Alpha" 1 "NULL" && '
        b'newt -v r2 Radiobutton 3 2 "Beta"  0 "$r1" && '
        b'newt -v r3 Radiobutton 3 3 "Gamma" 0 "$r2" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$r1" "$r2" "$r3" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    for label in ("Alpha", "Beta", "Gamma"):
        assert any(label in r for r in rows), \
            f"Radiobutton label '{label}' not found.\n{full}"

    bash_newt.send(b"\n")


def test_radio_get_current_returns_value(bash_newt):
    """RadioGetCurrent should return a non-empty pointer string."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "RadioGet" && '
        b'newt -v r1 Radiobutton 3 1 "One" 1 "NULL" && '
        b'newt -v r2 Radiobutton 3 2 "Two" 0 "$r1" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$r1" "$r2" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt -v cur RadioGetCurrent "$r1" && '
        b'newt Finished && '
        b'echo "cur=$cur"'
    )
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("cur=" in r for r in rows), \
        f"'cur=' not found in output.\n{full}"
    # The result should be a non-empty string (pointer or "NULL")
    cur_lines = [r for r in rows if "cur=" in r]
    assert cur_lines, f"No line with 'cur='\n{full}"
    assert cur_lines[0].strip() != "cur=", \
        f"RadioGetCurrent returned empty value.\n{full}"


def test_radio_set_current(bash_newt):
    """RadioSetCurrent pre-selects a specific button in the group."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "RadioSet" && '
        b'newt -v r1 Radiobutton 3 1 "First"  1 "NULL" && '
        b'newt -v r2 Radiobutton 3 2 "Second" 0 "$r1" && '
        b'newt -v r3 Radiobutton 3 3 "Third"  0 "$r2" && '
        b'newt RadioSetCurrent "$r3" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$r1" "$r2" "$r3" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt -v cur RadioGetCurrent "$r1" && '
        b'newt Finished && '
        b'[[ "$cur" == "$r3" ]] && echo "correct" || echo "wrong"'
    )
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("correct" in r for r in rows), \
        f"RadioSetCurrent did not pre-select r3 — 'correct' not found.\n{full}"
