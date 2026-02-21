"""Shared fixtures and helpers for newt functional tests.

Each test gets an isolated bash process running inside a pty (via pexpect).
Terminal output is captured and rendered by pyte so tests can assert on the
*visual* content of the screen rather than raw escape sequences.
"""

import pathlib
import pexpect
import pyte
import pytest

# Path to the compiled shared library (built by cmake from the repo root).
NEWT_SO = str(
    pathlib.Path(__file__).parent.parent / "build" / "src" / "newt.so"
)

# Virtual terminal dimensions passed to both pexpect and pyte.
COLS, ROWS = 80, 24


# ---------------------------------------------------------------------------
# Screen helpers
# ---------------------------------------------------------------------------

def drain(child: pexpect.spawn, initial_timeout: float = 2.0, drain_timeout: float = 0.3) -> bytes:
    """Read all pending pty output.

    Blocks up to *initial_timeout* seconds waiting for the first byte (giving
    newt time to render), then drains any remaining bytes quickly.
    """
    buf = b""
    try:
        buf += child.read_nonblocking(4096, timeout=initial_timeout)
    except (pexpect.TIMEOUT, pexpect.EOF):
        return buf
    try:
        while True:
            buf += child.read_nonblocking(4096, timeout=drain_timeout)
    except (pexpect.TIMEOUT, pexpect.EOF):
        pass
    return buf


def render(child: pexpect.spawn, **drain_kwargs) -> pyte.Screen:
    """Drain pty output and return a fully rendered *pyte.Screen*."""
    screen = pyte.Screen(COLS, ROWS)
    stream = pyte.ByteStream(screen)
    stream.feed(drain(child, **drain_kwargs))
    return screen


def screen_rows(screen: pyte.Screen) -> list[str]:
    """Return the rendered screen as a list of strings (one per row, rstripped)."""
    return [
        "".join(c.data for c in screen.buffer[row].values()).rstrip()
        for row in range(screen.lines)
    ]


def screen_text(screen: pyte.Screen) -> str:
    """Return all screen rows joined by newlines (useful for assertion messages)."""
    return "\n".join(screen_rows(screen))


# ---------------------------------------------------------------------------
# Fixtures
# ---------------------------------------------------------------------------

@pytest.fixture
def bash_newt():
    """Spawn a fresh bash session in a pty with the newt builtin loaded.

    Yields the *pexpect.spawn* child process; callers are responsible
    for sending commands and should call ``render()`` to inspect the screen.
    After the test the process is force-closed regardless of its state.
    """
    child = pexpect.spawn(
        "bash",
        ["--norc", "--noprofile"],
        encoding=None,          # raw bytes – required for pyte
        dimensions=(ROWS, COLS),
        timeout=10,
    )
    child.sendline(f"enable -f {NEWT_SO} newt".encode())
    # Wait briefly so the enable command completes before the test starts.
    try:
        child.expect(pexpect.TIMEOUT, timeout=0.5)
    except pexpect.EOF:
        pytest.fail(f"bash exited unexpectedly; is {NEWT_SO} built?")

    yield child

    child.close(force=True)
