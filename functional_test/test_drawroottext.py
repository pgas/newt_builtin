"""Functional tests for ``newt DrawRootText``.

DrawRootText places a string directly on the root (background) surface at a
given column/row.  These tests verify that the text actually appears at the
expected position on the rendered terminal screen.
"""

import pexpect
from conftest import render, screen_rows, screen_text


def test_drawroottext_top_left(bash_newt):
    """Text placed at col=0, row=0 should appear on the first screen row."""
    bash_newt.sendline(
        b"newt Init && "
        b'newt DrawRootText 0 0 "hello root" && '
        b"newt Refresh && "
        b"newt WaitForKey && "
        b"newt Finished"
    )

    # Capture the screen while newt is blocked waiting for a key.
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)

    assert any("hello root" in row for row in rows), (
        "'hello root' not found anywhere on screen.\n" + screen_text(screen)
    )
    # More precise: the text should be on the very first row.
    assert "hello root" in rows[0], (
        f"Expected 'hello root' on row 0, got: {rows[0]!r}\n"
        + screen_text(screen)
    )

    # Unblock WaitForKey so bash exits cleanly.
    bash_newt.send(b"\n")


def test_drawroottext_offset(bash_newt):
    """Text placed at col=5, row=2 should appear at the correct screen position."""
    bash_newt.sendline(
        b"newt Init && "
        b'newt DrawRootText 5 2 "offset text" && '
        b"newt Refresh && "
        b"newt WaitForKey && "
        b"newt Finished"
    )

    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)

    assert "offset text" in rows[2], (
        f"Expected 'offset text' on row 2, got: {rows[2]!r}\n"
        + screen_text(screen)
    )
    # Verify the text starts at column 5 (0-indexed).
    col = rows[2].index("offset text")
    assert col == 5, (
        f"Expected 'offset text' to start at col 5, found at col {col}\n"
        + screen_text(screen)
    )

    bash_newt.send(b"\n")
