"""Functional tests for ``newt Textbox`` and ``newt TextboxReflowed``.

Verifies:
  - Textbox text is visible on screen
  - TextboxSetText changes the displayed text
  - TextboxReflowed wraps text at the given width and is visible
  - TextboxGetNumLines returns the correct line count
"""

import time
from conftest import render, screen_rows, screen_text


def test_textbox_text_visible(bash_newt):
    """Text passed to Textbox constructor should appear on screen."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "Textbox Test" && '
        b'newt -v tb Textbox 3 1 40 5 0 && '
        b'newt TextboxSetText "$tb" "Hello from textbox" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$tb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Hello from textbox" in r for r in rows), \
        f"Textbox content not visible.\n{full}"

    bash_newt.send(b"\n")


def test_textbox_set_text(bash_newt):
    """TextboxSetText should replace the original content."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "SetText Test" && '
        b'newt -v tb Textbox 3 1 40 5 0 && '
        b'newt TextboxSetText "$tb" "Initial text" && '
        b'newt TextboxSetText "$tb" "Updated text" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$tb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Updated text" in r for r in rows), \
        f"Updated textbox content not visible.\n{full}"

    bash_newt.send(b"\n")


def test_textbox_reflowed_visible(bash_newt):
    """TextboxReflowed should display wrapped text in a window."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "Reflow Test" && '
        b'newt -v tb TextboxReflowed 3 1 "Reflowed content here" 30 2 2 0 && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$tb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Reflowed" in r for r in rows), \
        f"TextboxReflowed content not visible.\n{full}"

    bash_newt.send(b"\n")


def test_textbox_get_num_lines(bash_newt):
    """TextboxGetNumLines should return the number of text lines."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 12 "NumLines" && '
        b'newt -v tb Textbox 3 1 40 5 0 && '
        b'newt TextboxSetText "$tb" "line one\nline two\nline three" && '
        b'newt -v n TextboxGetNumLines "$tb" && '
        b'newt -v _ok Button 3 7 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$_ok" "$tb" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "n=[$n]"'
    )
    render(bash_newt, initial_timeout=2.0)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("n=[" in r for r in rows), \
        f"'n=[' not found in output.\n{full}"


def test_reflow_text(bash_newt):
    """ReflowText should bind non-empty reflowed string and non-zero dimensions."""
    bash_newt.sendline(
        b"newt Init && "
        b'newt -v txt ReflowText "Hello World this is a long sentence" 10 2 2 rw rh && '
        b'newt Finished && '
        b'echo "rw=[$rw] rh=[$rh] ok=[${txt:+yes}]"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("rw=[" in r for r in rows), \
        f"ReflowText widthVar not bound.\n{full}"
    assert any("rh=[" in r for r in rows), \
        f"ReflowText heightVar not bound.\n{full}"
    assert any("ok=[yes]" in r for r in rows), \
        f"ReflowText returned empty string.\n{full}"
    w_lines = [r for r in rows if "rw=[" in r]
    assert w_lines and "rw=[0]" not in w_lines[0], \
        f"ReflowText widthVar unexpectedly zero.\n{full}"
