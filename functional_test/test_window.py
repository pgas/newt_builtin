"""Functional tests for ``newt CenteredWindow`` and ``newt PopWindow``.

CenteredWindow opens a window centered on the screen with a title.
PopWindow closes the topmost window and refreshes the screen.
"""

import time
from conftest import render, screen_rows, screen_text


def test_centeredwindow_title_visible(bash_newt):
    """CenteredWindow should display a centered window with the given title."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt CenteredWindow 40 8 "Centered Title" && '
        b'newt -v lbl Label 3 2 "inside centered" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$lbl" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Centered Title" in r for r in rows), \
        f"CenteredWindow title not visible.\n{full}"
    assert any("inside centered" in r for r in rows), \
        f"Label inside centered window not visible.\n{full}"

    bash_newt.send(b"\n")


def test_popwindow_removes_top_window(bash_newt):
    """PopWindow should close the topmost window, revealing the one below."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        # First window (bottom)
        b'newt OpenWindow 5 3 50 12 "Bottom Win" && '
        b'newt -v lbl1 Label 3 1 "bottom-label" && '
        # Second window (top) — will be popped
        b'newt OpenWindow 10 5 30 6 "Top Win" && '
        b'newt -v lbl2 Label 3 1 "top-label" && '
        # Pop the top window
        b'newt PopWindow && '
        # Now only the bottom window should be visible.
        # Create a form in the bottom window to display it.
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$lbl1" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt PopWindow && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    # After PopWindow, "Bottom Win" should be visible, "Top Win" should not
    assert any("Bottom Win" in r for r in rows), \
        f"Bottom window title not visible after PopWindow.\n{full}"

    # The top label should NOT be visible after pop
    assert not any("top-label" in r for r in rows), \
        f"Top window label still visible after PopWindow.\n{full}"

    bash_newt.send(b"\n")
