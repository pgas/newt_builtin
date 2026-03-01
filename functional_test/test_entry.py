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
        b'newt -v e Entry 3 2 "hello" 30 && '
        b'newt -v f Form "" "" 0 && '
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
        b'newt -v e Entry 3 2 "world" 30 && '
        b'newt -v _ok Button 3 5 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$_ok" "$e" && '
        b'newt RunForm "$f"'
    )
    render(bash_newt, initial_timeout=2.0)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    # Read the value BEFORE FormDestroy (component still valid),
    # then clean up and echo the result.
    bash_newt.sendline(
        b'newt -v val EntryGetValue "$e" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
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
        b'newt -v e Entry 3 2 "original" 30 && '
        b'newt EntrySet "$e" "updated" 1 && '
        b'newt -v _ok Button 3 5 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$_ok" "$e" && '
        b'newt RunForm "$f"'
    )
    render(bash_newt, initial_timeout=2.0)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    # Read the value BEFORE FormDestroy (component still valid),
    # then clean up and echo the result.
    bash_newt.sendline(
        b'newt -v val EntryGetValue "$e" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("val=[updated]" in r for r in rows), \
        f"EntrySet did not update value to 'updated'.\n{full}"


def test_entry_set_flags_disabled(bash_newt):
    """EntrySetFlags with NEWT_FLAG_DISABLED (8) should prevent editing."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "EntryFlags" && '
        b'newt -v e Entry 3 2 "fixed" 30 && '
        b'newt EntrySetFlags "$e" 8 0 && '
        b'newt -v _ok Button 3 5 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$_ok" "$e" && '
        b'newt RunForm "$f"'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("fixed" in r for r in rows), \
        f"Entry with disabled flag not visible.\n{full}"

    # Type some text — it should NOT appear in the entry because it is disabled.
    bash_newt.send(b"ZZZZ")
    time.sleep(0.3)
    # Tab to the OK button and press Enter to exit the form.
    bash_newt.send(b"\t")
    time.sleep(0.1)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    # Read back the entry value — it must still be "fixed".
    bash_newt.sendline(
        b'newt -v val EntryGetValue "$e" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
    screen2 = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows2 = screen_rows(screen2)
    full2 = screen_text(screen2)

    assert any("val=[fixed]" in r for r in rows2), \
        f"Disabled entry should still contain 'fixed' after typing.\n{full2}"


def test_entry_set_filter_blocks_digits(bash_newt):
    """EntrySetFilter should block characters when the filter function returns non-zero."""
    # The filter bash function receives NEWT_CH (decimal key code).
    # ASCII digits '0'-'9' are codes 48-57.  We return 1 (failure) for those
    # so the shim returns 0 (block), and return 0 (success) for all others
    # so the shim passes the character through.
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "FilterTest" && '
        b'block_digits() { [[ $NEWT_CH -ge 48 && $NEWT_CH -le 57 ]] && return 1; return 0; } && '
        b'newt -v e Entry 3 2 "" 30 && '
        b'newt EntrySetFilter "$e" block_digits && '
        b'newt -v _ok Button 3 5 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$e" "$_ok" && '
        b'newt RunForm "$f"'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("FilterTest" in r for r in rows), \
        f"Filter test window not visible.\n{full}"

    # Type a mix of letters and digits — only letters should get through
    bash_newt.send(b"a1b2c3")
    time.sleep(0.3)
    # Tab to OK and press Enter to exit the form
    bash_newt.send(b"\t")
    time.sleep(0.1)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    # Read back the entry value
    bash_newt.sendline(
        b'newt -v val EntryGetValue "$e" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
    screen2 = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows2 = screen_rows(screen2)
    full2 = screen_text(screen2)

    assert any("val=[abc]" in r for r in rows2), \
        f"EntrySetFilter should have blocked digits; expected 'abc'.\n{full2}"

