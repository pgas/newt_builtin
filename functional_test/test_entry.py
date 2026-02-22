"""Functional tests for ``newt Entry`` widget and related commands.

Covers: Entry, EntryGetValue, EntrySet, EntrySetFlags.
"""

import time
from conftest import render, screen_rows, screen_text


def test_entry_visible(bash_newt):
    """An Entry widget should appear in the window."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "Entry Test" && '
        b'newt -v e Entry 3 2 "hello" 30 0 "" 0 && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$e" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("hello" in r for r in rows), \
        f"Entry default text 'hello' not visible.\n{full}"

    bash_newt.send(b"\n")


def test_entry_getvalue(bash_newt):
    """EntryGetValue should return the current text in an Entry."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "EntryGV" && '
        b'newt -v e Entry 3 2 "world" 30 0 "" 0 && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$e" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt -v val EntryGetValue "$e" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("val=[" in r for r in rows), \
        f"'val=[' not found in output.\n{full}"
    assert any("val=[world]" in r for r in rows), \
        f"EntryGetValue did not return 'world'.\n{full}"


def test_entry_set_changes_value(bash_newt):
    """EntrySet should update the Entry's value and EntryGetValue reflects it."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "EntrySet" && '
        b'newt -v e Entry 3 2 "original" 30 0 "" 0 && '
        b'newt EntrySet "$e" "updated" 1 && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$e" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt -v val EntryGetValue "$e" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("val=[updated]" in r for r in rows), \
        f"EntrySet did not update value to 'updated'.\n{full}"


def test_entry_set_flags_disabled(bash_newt):
    """EntrySetFlags with NEWT_FLAG_DISABLED should prevent editing."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "EntryFlags" && '
        b'newt -v e Entry 3 2 "fixed" 30 0 "" 0 && '
        b'newt EntrySetFlags "$e" 2 1 && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$e" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("fixed" in r for r in rows), \
        f"Entry with disabled flag not visible.\n{full}"

    bash_newt.send(b"\n")
