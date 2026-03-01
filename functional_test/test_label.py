"""Functional tests for the ``newt Label`` widget.

Opens a window containing a Label and verifies that both the window title and
the label text appear on the rendered terminal screen.
"""

import pexpect
from conftest import render, screen_rows, screen_text


def test_label_text_visible(bash_newt):
    """Label text and window title should both appear on the screen."""
    # Inline bash reproduces the label.sh example: open a window, create a
    # label widget, add it to a form, then run the form (which blocks on input).
    script = (
        b"newt Init && "
        b"newt Cls && "
        b'newt OpenWindow 10 5 40 7 "Label Window" && '
        b'newt -v l1 Label 10 1 "Hello, world!" && '
        b"newt -v myform Form '' '' 0 && "
        b'newt FormAddComponent "$myform" "$l1" && '
        b'newt RunForm "$myform" && '
        b'newt FormDestroy "$myform" && '
        b"newt Finished"
    )
    bash_newt.sendline(script)

    # Capture the screen while RunForm is blocking for input.
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Label Window" in row for row in rows), (
        "'Label Window' (window title) not found on screen.\n" + full
    )
    assert any("Hello, world!" in row for row in rows), (
        "'Hello, world!' (label text) not found on screen.\n" + full
    )

    # Send Enter to unblock RunForm.
    bash_newt.send(b"\n")


def test_label_set_text(bash_newt):
    """LabelSetText should update the label text visible on screen."""
    # Use an initial text whose every character differs from "updated text"
    # so that a differential terminal refresh sends all 12 updated cells,
    # giving pyte a complete fresh repaint to assert on.
    setup = (
        b"newt Init && "
        b"newt Cls && "
        b'newt OpenWindow 10 5 40 7 "Update Test" && '
        b'newt -v l1 Label 10 1 "HELLO WORLD!" && '
        b'newt -v _cont Button 5 4 "Continue" && '
        b"newt -v myform Form '' '' 0 && "
        b'newt FormAddComponents "$myform" "$_cont" "$l1" && '
        b'newt RunForm "$myform"'
    )
    bash_newt.sendline(setup)

    # Confirm initial render shows "HELLO WORLD!".
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    assert any("HELLO WORLD!" in row for row in rows), (
        "'HELLO WORLD!' not found on initial render.\n" + screen_text(screen)
    )

    # Press Enter to return from RunForm, then update the label and refresh.
    bash_newt.send(b"\r")
    bash_newt.sendline(
        b'newt LabelSetText "$l1" "updated text" && '
        b"newt Refresh && "
        b"newt WaitForKey && "
        b'newt FormDestroy "$myform" && '
        b"newt Finished"
    )

    screen2 = render(bash_newt, initial_timeout=2.0)
    rows2 = screen_rows(screen2)
    assert any("updated text" in row for row in rows2), (
        "'updated text' not found after LabelSetText.\n" + screen_text(screen2)
    )

    bash_newt.send(b"\r")
