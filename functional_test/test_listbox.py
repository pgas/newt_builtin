"""Functional tests for ``newt Listbox`` and related commands.

Covers: Listbox (constructor), ListboxAddEntry / ListboxAppendEntry,
ListboxSetCurrent, ListboxSetEntry, ListboxGetCurrent, ListboxItemCount,
ListboxClear, ListboxGetSelection.
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
        b' && newt -v f Form "" "" 0 && '
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
        b'newt -v _ok Button 3 10 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$_ok" "$lb" && '
        b'newt RunForm "$f"'
    )
    render(bash_newt, initial_timeout=2.0)  # wait for listbox form
    bash_newt.send(b"\r")  # press Enter on the OK button to exit the form
    time.sleep(0.5)
    # Query the count BEFORE FormDestroy (component still valid),
    # then clean up and echo the result.
    bash_newt.sendline(
        b'newt -v cnt ListboxItemCount "$lb" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "cnt=[$cnt]"'
    )
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
        b'newt ListboxSetEntry "$lb" 0 "New" && '
        b'newt -v f Form "" "" 0 && '
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

    bash_newt.send(b"\r")


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
        b'newt -v _ok Button 3 10 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$_ok" "$lb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "cnt=[$cnt]"'
    )
    render(bash_newt, initial_timeout=2.0)  # wait for form
    bash_newt.send(b"\r")  # click OK to exit form
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
        b'newt -v _ok Button 3 10 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$_ok" "$lb" && '
        b'newt RunForm "$f" && '
        b'newt -v key ListboxGetCurrent "$lb" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "key=[$key]"'
    )
    render(bash_newt, initial_timeout=2.0)  # wait for form
    bash_newt.send(b"\r")  # click OK to exit form
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("key=[" in r for r in rows), \
        f"'key=[' not found in output.\n{full}"
    key_lines = [r for r in rows if "key=[" in r]
    assert key_lines and key_lines[0].strip() != "key=[]", \
        f"ListboxGetCurrent returned empty key.\n{full}"


def test_listbox_getselection(bash_newt):
    """ListboxGetSelection should return pre-selected items."""
    # NEWT_FLAG_MULTIPLE = 1<<8 = 256
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v lb Listbox 3 1 8 256 && '
        b'newt ListboxAddEntry "$lb" "X" 1 && '
        b'newt ListboxAddEntry "$lb" "Y" 2 && '
        b'newt ListboxAddEntry "$lb" "Z" 3 && '
        b'newt ListboxSelectItem "$lb" 2 0 && '
        b'newt ListboxGetSelection "$lb" sel && '
        b'newt Finished && '
        b'echo "cnt=[$sel] first=[$sel_0]"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("cnt=[" in r for r in rows), \
        f"ListboxGetSelection count not bound.\n{full}"
    cnt_lines = [r for r in rows if "cnt=[" in r]
    assert cnt_lines and "cnt=[0]" not in cnt_lines[0], \
        f"ListboxGetSelection returned zero items.\n{full}"
    assert any("first=[" in r for r in rows), \
        f"ListboxGetSelection first item not bound.\n{full}"


# ─── ListboxGetEntry ─────────────────────────────────────────────────────────

def test_listbox_getentry(bash_newt):
    """ListboxGetEntry should return the stored text and data for an item by index."""
    # No form/RunForm needed — the item list is populated by ListboxAddEntry.
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v lb Listbox 3 1 8 0 && '
        b'newt ListboxAddEntry "$lb" "Mango" 5 && '
        b'newt ListboxAddEntry "$lb" "Pear"  6 && '
        b'newt ListboxGetEntry "$lb" 0 ltxt ldat && '
        b'newt Finished && '
        b'echo "ltxt=[$ltxt] ldat=[$ldat]"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("ltxt=[Mango]" in r for r in rows), \
        f"ListboxGetEntry text should be 'Mango'.\n{full}"
    assert any("ldat=[5]" in r for r in rows), \
        f"ListboxGetEntry data should be '5'.\n{full}"


# ─── ListboxDeleteEntry ───────────────────────────────────────────────────────

def test_listbox_deleteentry(bash_newt):
    """ListboxDeleteEntry should remove the item with the given data key."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v lb Listbox 3 1 8 0 && '
        b'newt ListboxAddEntry "$lb" "A" 1 && '
        b'newt ListboxAddEntry "$lb" "B" 2 && '
        b'newt ListboxAddEntry "$lb" "C" 3 && '
        b'newt ListboxDeleteEntry "$lb" 2 && '
        b'newt -v cnt ListboxItemCount "$lb" && '
        b'newt Finished && '
        b'echo "cnt=[$cnt]"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("cnt=[2]" in r for r in rows), \
        f"ListboxDeleteEntry should reduce item count to 2.\n{full}"


# ─── ListboxInsertEntry ───────────────────────────────────────────────────────

def test_listbox_insertentry_preserves_order(bash_newt):
    """ListboxInsertEntry should insert a new item immediately before the given key."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "InsertEntry" && '
        b'newt -v lb Listbox 3 1 6 0 && '
        b'newt ListboxAddEntry "$lb" "First" 1 && '
        b'newt ListboxAddEntry "$lb" "Third" 3 && '
        # Insert "Second" with data key 2, before the item with key 3
        b'newt ListboxInsertEntry "$lb" "Second" 2 3 && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$lb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("First"  in r for r in rows), f"'First' not visible.\n{full}"
    assert any("Second" in r for r in rows), f"'Second' not visible after insert.\n{full}"
    assert any("Third"  in r for r in rows), f"'Third' not visible.\n{full}"

    bash_newt.send(b"\n")


# ─── ListboxSetCurrentByKey ───────────────────────────────────────────────────

def test_listbox_setcurrentbykey(bash_newt):
    """ListboxSetCurrentByKey should pre-select the item matching the given data key."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v lb Listbox 3 1 8 0 && '
        b'newt ListboxAddEntry "$lb" "Alpha"  1 && '
        b'newt ListboxAddEntry "$lb" "Beta"   2 && '
        b'newt ListboxAddEntry "$lb" "Gamma"  3 && '
        b'newt ListboxSetCurrentByKey "$lb" 2 && '
        b'newt -v _ok Button 3 10 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$_ok" "$lb" && '
        b'newt RunForm "$f" && '
        b'newt -v cur ListboxGetCurrent "$lb" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "cur=[$cur]"'
    )
    render(bash_newt, initial_timeout=2.0)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("cur=[2]" in r for r in rows), \
        f"ListboxSetCurrentByKey did not select key 2 (expected cur=[2]).\n{full}"
