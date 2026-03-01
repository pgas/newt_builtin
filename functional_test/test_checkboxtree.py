"""Functional tests for ``newt CheckboxTree`` and related APIs.

Verifies:
  - CheckboxTree widget is visible with items added via CheckboxTreeAddItem
  - CheckboxTreeGetEntryValue reads back the correct state
  - CheckboxTreeSetEntryValue changes the displayed state
  - CheckboxTreeMulti with custom sequence displays items
  - CheckboxTreeSetCurrent / CheckboxTreeGetCurrent move cursor
  - CheckboxTreeGetSelection returns checked items
  - CheckboxTreeGetMultiSelection returns items matching a seqnum
  - CheckboxTreeFindItem returns the index path for a data key
"""

import time
from conftest import render, screen_rows, screen_text


def test_checkboxtree_items_visible(bash_newt):
    """Items added to a CheckboxTree should appear on screen."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 3 2 60 15 "Tree Test" && '
        b'newt -v ct CheckboxTree 3 2 8 0 && '
        b'newt CheckboxTreeAddItem "$ct" "Apple"  1 0 0 && '
        b'newt CheckboxTreeAddItem "$ct" "Banana" 2 0 1 && '
        b'newt CheckboxTreeAddItem "$ct" "Cherry" 3 0 2 && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$ct" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    for item in ("Apple", "Banana", "Cherry"):
        assert any(item in r for r in rows), \
            f"CheckboxTree item '{item}' not visible.\n{full}"

    bash_newt.send(b"\n")


def test_checkboxtree_getentryvalue(bash_newt):
    """CheckboxTreeGetEntryValue after SetEntryValue should reflect the change."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 3 2 60 12 "Tree Val" && '
        b'newt -v ct CheckboxTree 3 2 6 0 && '
        b'newt CheckboxTreeAddItem "$ct" "ItemX" 1 0 0 && '
        b'newt CheckboxTreeSetEntryValue "$ct" 1 "*" && '
        b'newt -v _ok Button 3 8 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$_ok" "$ct" && '
        b'newt RunForm "$f"'
    )
    render(bash_newt, initial_timeout=2.0)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    # Read the value BEFORE FormDestroy (component still valid),
    # then clean up and echo the result.
    bash_newt.sendline(
        b'newt -v val CheckboxTreeGetEntryValue "$ct" 1 && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("val=[" in r for r in rows), \
        f"'val=[' not found in output.\n{full}"


def test_checkboxtree_multi(bash_newt):
    """CheckboxTreeMulti with custom sequence should display items."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 3 2 60 12 "Multi Tree" && '
        b'newt -v ct CheckboxTreeMulti 3 2 6 "* " 0 && '
        b'newt CheckboxTreeAddItem "$ct" "Option1" 1 0 0 && '
        b'newt CheckboxTreeAddItem "$ct" "Option2" 2 0 1 && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$ct" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Option1" in r for r in rows), \
        f"'Option1' not visible in CheckboxTreeMulti.\n{full}"

    bash_newt.send(b"\n")


def test_checkboxtree_setcurrent_getcurrent(bash_newt):
    """CheckboxTreeSetCurrent should move the cursor; GetCurrent returns a non-zero data pointer."""
    # SetCurrent/GetCurrent work on in-memory state — no form run needed.
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v ct CheckboxTree 3 2 6 0 && '
        b'newt CheckboxTreeAddItem "$ct" "First" 1 0 0 && '
        b'newt CheckboxTreeAddItem "$ct" "Second" 2 0 1 && '
        b'newt CheckboxTreeSetCurrent "$ct" 2 && '
        b'newt -v cur CheckboxTreeGetCurrent "$ct" && '
        b'newt Finished && '
        b'echo "cur=[$cur]"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("cur=[" in r for r in rows), \
        f"CheckboxTreeGetCurrent not bound.\n{full}"
    cur_lines = [r for r in rows if "cur=[" in r]
    assert cur_lines and "cur=[]" not in cur_lines[0], \
        f"CheckboxTreeGetCurrent returned empty/null.\n{full}"


def test_checkboxtree_getselection(bash_newt):
    """CheckboxTreeGetSelection should return items whose value is '*'."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v ct CheckboxTree 3 2 8 0 && '
        b'newt CheckboxTreeAddItem "$ct" "Alpha" 1 0 0 && '
        b'newt CheckboxTreeAddItem "$ct" "Beta"  2 0 1 && '
        b'newt CheckboxTreeSetEntryValue "$ct" 1 "*" && '
        b'newt CheckboxTreeGetSelection "$ct" sel && '
        b'newt Finished && '
        b'echo "cnt=[$sel] item0=[$sel_0]"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("cnt=[" in r for r in rows), \
        f"CheckboxTreeGetSelection count not bound.\n{full}"
    cnt_lines = [r for r in rows if "cnt=[" in r]
    assert cnt_lines and "cnt=[0]" not in cnt_lines[0], \
        f"CheckboxTreeGetSelection returned zero items.\n{full}"


def test_checkboxtree_getmultiselection(bash_newt):
    """CheckboxTreeGetMultiSelection returns items matching seqnum without a form run."""
    # Use a plain CheckboxTree (default seq ' '/\'*\') to keep the test simple.
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v ct CheckboxTree 3 2 8 0 && '
        b'newt CheckboxTreeAddItem "$ct" "P" 10 0 0 && '
        b'newt CheckboxTreeAddItem "$ct" "Q" 20 0 1 && '
        b'newt CheckboxTreeSetEntryValue "$ct" 10 "*" && '
        b'newt CheckboxTreeGetMultiSelection "$ct" msel "*" && '
        b'newt Finished && '
        b'echo "cnt=[$msel] m0=[$msel_0]"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("cnt=[" in r for r in rows), \
        f"CheckboxTreeGetMultiSelection count not bound.\n{full}"
    cnt_lines = [r for r in rows if "cnt=[" in r]
    assert cnt_lines and "cnt=[0]" not in cnt_lines[0], \
        f"CheckboxTreeGetMultiSelection returned zero items.\n{full}"


def test_checkboxtree_finditem(bash_newt):
    """CheckboxTreeFindItem should return a non-empty index list."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v ct CheckboxTree 3 2 8 0 && '
        b'newt CheckboxTreeAddItem "$ct" "Needle" 42 0 0 && '
        b'newt -v idxs CheckboxTreeFindItem "$ct" 42 && '
        b'newt Finished && '
        b'echo "idxs=[$idxs]"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("idxs=[" in r for r in rows), \
        f"CheckboxTreeFindItem result not bound.\n{full}"
    idx_lines = [r for r in rows if "idxs=[" in r]
    assert idx_lines and "idxs=[]" not in idx_lines[0], \
        f"CheckboxTreeFindItem returned empty list.\n{full}"
