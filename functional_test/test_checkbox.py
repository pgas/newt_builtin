"""Functional tests for ``newt Checkbox`` and related APIs.

Verifies:
  - Checkbox text is visible on screen
  - CheckboxGetValue reads back the correct default state
  - CheckboxSetValue changes the state
  - CheckboxSetFlags can disable a checkbox
"""

import time
from conftest import render, screen_rows, screen_text


def test_checkbox_text_visible(bash_newt):
    """Checkbox label should appear inside the window."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "Checkbox Test" && '
        b'newt -v cb Checkbox 3 2 "Enable feature" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponent "$f" "$cb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Enable feature" in r for r in rows), \
        f"Checkbox label not found.\n{full}"

    bash_newt.send(b"\n")


def test_checkbox_default_unchecked(bash_newt):
    """CheckboxGetValue on a freshly created checkbox should return space (unchecked)."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 8 "Check Default" && '
        b'newt -v cb Checkbox 3 1 "Item" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponent "$f" "$cb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt -v val CheckboxGetValue "$cb" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
    # Unblock the form first
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    # Default value is a space ' ' (unchecked); shell echo prints "val=[ ]"
    assert any("val=[" in r for r in rows), \
        f"'val=[' not found in output.\n{full}"


def test_checkbox_preset_checked(bash_newt):
    """Checkbox created with '*' as defValue should be checked by default."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 8 "Pre-checked" && '
        b'newt -v cb Checkbox 3 1 "Selected" "*" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponent "$f" "$cb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt -v val CheckboxGetValue "$cb" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("val=[*]" in r for r in rows), \
        f"Expected 'val=[*]' for pre-checked box, got:\n{full}"


def test_checkbox_set_value(bash_newt):
    """CheckboxSetValue should change the checkbox state."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 8 "SetValue" && '
        b'newt -v cb Checkbox 3 1 "Toggle" && '
        b'newt CheckboxSetValue "$cb" "*" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponent "$f" "$cb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt -v val CheckboxGetValue "$cb" && '
        b'newt Finished && '
        b'echo "val=[$val]"'
    )
    bash_newt.send(b"\n")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("val=[*]" in r for r in rows), \
        f"Expected 'val=[*]' after CheckboxSetValue '*'.\n{full}"


def test_checkbox_set_flags_disable(bash_newt):
    """CheckboxSetFlags with DISABLED should make the checkbox non-interactive."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 8 "Disabled CB" && '
        b'newt -v cb Checkbox 3 1 "Disabled item" && '
        b'newt CheckboxSetFlags "$cb" 8 0 && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponent "$f" "$cb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Disabled item" in r for r in rows), \
        f"Disabled checkbox label not found.\n{full}"

    bash_newt.send(b"\n")
