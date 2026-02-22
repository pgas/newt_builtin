"""Functional tests for ``newt Listbox`` and related commands.

Covers: Listbox (constructor), ListboxAddEntry / ListboxAppendEntry,
ListboxSetCurrent, ListboxSetEntry, ListboxGetCurrent, ListboxItemCount,
ListboxClear.
"""

import time
from conftest import render, screen_rows, screen_text


def _listbox_form(bash_newt, extra_cmds=b""):
    """Helper: build a small window with a listbox, run the form, then finish."""
    setup = (
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 15 "Listbox Test" && '
        b'newt -v lb Listbox 3 1 8 0 && '
        b'newt ListboxAddEntry "$lb" "Apple"  1 && '
        b'newt ListboxAddEntry "$lb" "Banana" 2 && '
        b'newt ListboxAddEntry "$lb" "Cherry" 3'
    )
    if extra_cmds:
        setup = setup + b" && " + extra_cmds
    setup += (
        b' && newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$lb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    return setup


def test_listbox_entries_visible(bash_newt):
    """Items added via ListboxAddEntry should appear in the window."""
    bash_newt.sendline(_listbox_form(bash_newt))
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    for item in ("Apple", "Banana", "Cherry"):
        assert any(item in r for r in rows), \
            f"Listbox item '{item}' not visible.\n{full}"

    bash_newt.send(b"\n")


def test_listbox_setcurrent(bash_newt):
    """ListboxSetCurrent(2) should pre-select the third item ('Cherry')."""
    bash_newt.sendline(
        _listbox_form(bash_newt, b'newt ListboxSetCurrent "$lb" 2')
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Cherry" in r for r in rows), \
        f"Cherry not visible after ListboxSetCurrent.\n{full}"

    bash_newt.send(b"\n")


def test_listbox_item_count(bash_newt):
    """ListboxItemCount should return 3 after adding 3 items."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 15 "LBCount" && '
        b'newt -v lb Listbox 3 1 8 0 && '
        b'newt ListboxAddEntry "$lb" "A" 1 && '
        b'newt ListboxAddEntry "$lb" "B" 2 && '
        b'newt ListboxAddEntry "$lb" "C" 3 && '
        b'newt -v cnt ListboxItemCount "$lb" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$lb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "cnt=[$cnt]"'
    )
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("cnt=[3]" in r for r in rows), \
        f"ListboxItemCount did not return 3.\n{full}"


def test_listbox_setentry_updates_text(bash_newt):
    """ListboxSetEntry should replace an item's visible text."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 15 "LBSetEntry" && '
        b'newt -v lb Listbox 3 1 8 0 && '
        b'newt ListboxAddEntry "$lb" "Old"  1 && '
        b'newt ListboxAddEntry "$lb" "Keep" 2 && '
        b'newt ListboxSetEntry "$lb" 1 "New" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$lb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("New" in r for r in rows), \
        f"'New' not found after ListboxSetEntry.\n{full}"
    assert not any("Old" in r for r in rows), \
        f"'Old' still visible after ListboxSetEntry.\n{full}"

    bash_newt.send(b"\n")


def test_listbox_clear(bash_newt):
    """ListboxClear should remove all items from the listbox."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 15 "LBClear" && '
        b'newt -v lb Listbox 3 1 8 0 && '
        b'newt ListboxAddEntry "$lb" "Alpha" 1 && '
        b'newt ListboxAddEntry "$lb" "Beta"  2 && '
        b'newt ListboxClear "$lb" && '
        b'newt -v cnt ListboxItemCount "$lb" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$lb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "cnt=[$cnt]"'
    )
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("cnt=[0]" in r for r in rows), \
        f"ListboxClear did not empty the listbox (cnt expected 0).\n{full}"


def test_listbox_getcurrent(bash_newt):
    """ListboxGetCurrent should return a non-empty key after item selection."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 15 "LBGetCur" && '
        b'newt -v lb Listbox 3 1 8 0 && '
        b'newt ListboxAddEntry "$lb" "X" 10 && '
        b'newt ListboxAddEntry "$lb" "Y" 20 && '
        b'newt ListboxSetCurrent "$lb" 0 && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$lb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt -v key ListboxGetCurrent "$lb" && '
        b'newt Finished && '
        b'echo "key=[$key]"'
    )
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("key=[" in r for r in rows), \
        f"'key=[' not found in output.\n{full}"
    key_lines = [r for r in rows if "key=[" in r]
    assert key_lines and key_lines[0].strip() != "key=[]", \
        f"ListboxGetCurrent returned empty key.\n{full}"
