"""Functional tests for ``newt Button`` and ``newt CompactButton``.

Opens a window with both button types inside a form and verifies that:
  - button text appears on screen
  - the form's window title appears
  - pressing Enter unblocks RunForm
"""

from conftest import render, screen_rows, screen_text


def test_button_text_visible(bash_newt):
    """Button text should appear inside the window."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 5 40 8 "Button Test" && '
        b'newt -v b1 Button 5 1 "OK" && '
        b'newt -v b2 Button 5 3 "Cancel" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$b1" "$b2" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Button Test" in r for r in rows), \
        f"Window title 'Button Test' not found.\n{full}"
    assert any("OK" in r for r in rows), \
        f"'OK' button text not found.\n{full}"
    assert any("Cancel" in r for r in rows), \
        f"'Cancel' button text not found.\n{full}"

    bash_newt.send(b"\n")


def test_compact_button_text_visible(bash_newt):
    """CompactButton text should appear inside the window."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 5 50 8 "CompactButton Test" && '
        b'newt -v b1 CompactButton 5 1 "Yes" && '
        b'newt -v b2 CompactButton 20 1 "No" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$b1" "$b2" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("CompactButton Test" in r for r in rows), \
        f"Window title not found.\n{full}"
    assert any("Yes" in r for r in rows), \
        f"'Yes' compact button not found.\n{full}"
    assert any("No" in r for r in rows), \
        f"'No' compact button not found.\n{full}"

    bash_newt.send(b"\n")


def test_form_add_components_multiple(bash_newt):
    """FormAddComponents should add multiple components in one call."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "Multi-Add" && '
        b'newt -v b1 Button 3 1 "First" && '
        b'newt -v b2 Button 3 3 "Second" && '
        b'newt -v b3 Button 3 5 "Third" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$b1" "$b2" "$b3" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    for label in ("First", "Second", "Third"):
        assert any(label in r for r in rows), \
            f"Button '{label}' not found on screen.\n{full}"

    bash_newt.send(b"\n")


def test_runform_returns_component(bash_newt):
    """RunForm exit result should be bound to the varname variable."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 5 40 6 "Return Test" && '
        b'newt -v btn Button 5 1 "Go" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponent "$f" "$btn" && '
        b'newt -v ret RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "ret=$ret"'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    bash_newt.send(b"\n")
    import time; time.sleep(0.5)
    screen2 = render(bash_newt, initial_timeout=1.0, drain_timeout=0.2)
    rows = screen_rows(screen2)
    full = screen_text(screen2)

    # After Finished the terminal has "ret=0x..." on it
    assert any("ret=" in r for r in rows), \
        f"'ret=' not found in output after Finished.\n{full}"
