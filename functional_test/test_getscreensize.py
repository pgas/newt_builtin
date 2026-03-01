"""Functional tests for ``newt GetScreenSize``.

Verifies that GetScreenSize returns positive integer dimensions matching
the pty size configured by conftest.py (80×24).
"""

from conftest import render, screen_rows, screen_text, COLS, ROWS


def test_getscreensize_returns_positive(bash_newt):
    """GetScreenSize should bind cols and rows to the pty dimensions."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt GetScreenSize C R && '
        b'newt Finished && '
        b'echo "C=$C R=$R"'
    )
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    cr_lines = [r for r in rows if "C=" in r and "R=" in r]
    assert cr_lines, f"GetScreenSize output not found.\n{full}"

    # Parse and verify
    import re
    m = re.search(r"C=(\d+)\s+R=(\d+)", cr_lines[0])
    assert m, f"Could not parse C=N R=N from: {cr_lines[0]}\n{full}"
    c, r = int(m.group(1)), int(m.group(2))
    assert c == COLS, f"Expected cols={COLS}, got {c}"
    assert r == ROWS, f"Expected rows={ROWS}, got {r}"


def test_getscreensize_too_few_args(bash_newt):
    """GetScreenSize with only one argument should fail."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt GetScreenSize C; echo "rc=$?"'
    )
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    # The builtin should have returned non-zero (EXECUTION_FAILURE)
    # but even though newt Init was already called, let's just check rc
    assert any("rc=1" in r for r in rows), \
        f"GetScreenSize with 1 arg should fail (rc=1).\n{full}"
