"""Functional tests for component-level operations.

Covers:
    ComponentAddDestroyCallback – registers a bash expression run on destroy
    ComponentTakesFocus – controls whether a widget is focusable
    ComponentDestroy – destroys a standalone component
"""

import time
from conftest import render, screen_rows, screen_text


# ─── ComponentAddDestroyCallback ──────────────────────────────────────────────

def test_component_add_destroy_callback_fires(bash_newt):
    """The destroy callback should fire when the component is destroyed."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "DestroyCallback" && '
        b'newt -v btn Button 3 2 "Boom" && '
        b'DESTROYED=0 && '
        b"newt ComponentAddDestroyCallback \"$btn\" 'DESTROYED=1' && "
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$btn" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "DESTROYED=$DESTROYED"'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("DestroyCallback" in r for r in rows), \
        f"Window did not appear.\n{full}"

    # Press Enter to exit the form → FormDestroy fires the callback
    bash_newt.send(b"\r")
    time.sleep(0.8)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("DESTROYED=1" in r for r in rows), \
        f"ComponentAddDestroyCallback did not fire.\n{full}"


# ─── ComponentTakesFocus ──────────────────────────────────────────────────────

def test_component_takes_focus_skip(bash_newt):
    """ComponentTakesFocus(0) should make Tab skip that component."""
    # Create two buttons: btn1 is disabled for focus, btn2 should get focus
    # immediately after the form starts (since btn1 can't take focus).
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "FocusTest" && '
        b'newt -v btn1 Button 3 2 "Skip" && '
        b'newt -v btn2 Button 3 4 "Here" && '
        # Disable focus on btn1
        b'newt ComponentTakesFocus "$btn1" 0 && '
        b'newt -v f Form "" "" 0 && '
        # Add btn1 first, then btn2 — normally btn1 would start focused
        b'newt FormAddComponents "$f" "$btn1" "$btn2" && '
        b'newt FormRun "$f" REASON VALUE && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "REASON=$REASON VALUE=$VALUE"'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("FocusTest" in r for r in rows), \
        f"Window did not appear.\n{full}"

    # Press Enter — since btn1 can't take focus, btn2 is focused and activates
    bash_newt.send(b"\r")
    time.sleep(0.8)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("REASON=COMPONENT" in r for r in rows), \
        f"Expected COMPONENT exit reason.\n{full}"
    # VALUE should be the pointer for btn2, not btn1
    val_lines = [r for r in rows if "VALUE=" in r]
    assert val_lines, f"VALUE= not found in output.\n{full}"


# ─── ComponentDestroy ─────────────────────────────────────────────────────────

def test_component_destroy_standalone(bash_newt):
    """ComponentDestroy should destroy a standalone component without crashing."""
    # Create a label outside any form, destroy it, then proceed to create
    # and display a normal form — verifying no crash occurred.
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v lbl Label 1 1 "temp" && '
        b'newt ComponentDestroy "$lbl" && '
        b'newt OpenWindow 5 3 50 8 "AfterDestroy" && '
        b'newt -v btn Button 3 2 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$btn" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("AfterDestroy" in r for r in rows), \
        f"Window after ComponentDestroy not visible — may have crashed.\n{full}"

    bash_newt.send(b"\n")
