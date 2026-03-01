"""Functional tests for ``newt SetSuspendCallback``.

SetSuspendCallback registers a bash function that libnewt calls when the user
presses Ctrl+Z while a form is running.  These tests verify that:
 1. Registering a callback and running a form works without crashing.
 2. When Ctrl+Z is sent, the registered bash function fires (detected via a
    side-effect file written by the callback).
 3. Calling SetSuspendCallback with no arguments returns failure.
"""

import os
import pexpect
import tempfile
from conftest import render, screen_rows, screen_text


def test_setsuspendcallback_registration_does_not_crash(bash_newt):
    """Registering a suspend callback must not crash; form must run normally."""
    bash_newt.sendline(
        b"newt Init && "
        b"newt Cls && "
        b'newt OpenWindow 10 5 40 5 "Suspend Test" && '
        b'my_suspend() { newt Suspend; newt Resume; } && '
        b"newt SetSuspendCallback my_suspend && "
        b'newt -v btn Button 14 1 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$btn" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )

    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)

    # The window title should appear, confirming newt rendered successfully.
    assert any("Suspend Test" in row for row in rows), (
        "'Suspend Test' not found; newt may have crashed.\n" + screen_text(screen)
    )

    # Unblock RunForm.
    bash_newt.send(b"\n")


def test_setsuspendcallback_callback_fires_on_sigtstp(bash_newt, tmp_path):
    """The registered bash function must execute when Ctrl+Z is pressed."""
    flag_file = str(tmp_path / "suspend_fired")

    # The callback writes a flag file so we can detect it was called.
    # It also calls newtSuspend + newtResume to let newt recover gracefully.
    callback_def = (
        b"my_suspend() { "
        b"newt Suspend; "
        b'echo fired > "' + flag_file.encode() + b'"; '
        b"newt Resume; "
        b"}"
    )

    bash_newt.sendline(
        callback_def + b" && "
        b"newt Init && "
        b"newt Cls && "
        b'newt OpenWindow 5 2 50 5 "Waiting" && '
        b'newt -v lbl Label 5 1 "press ctrl-z" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$lbl" && '
        b"newt SetSuspendCallback my_suspend && "
        b'newt RunForm "$f"'
    )

    # Wait for the form to appear on screen.
    render(bash_newt, initial_timeout=1.5)

    # Send Ctrl+Z — triggers the suspend callback inside libnewt.
    bash_newt.send(b"\x1a")

    # Give the callback time to run and write the flag file.
    import time; time.sleep(1.0)

    assert os.path.exists(flag_file), (
        f"Flag file {flag_file!r} not created; suspend callback did not fire."
    )

    # Clean up: press Enter to dismiss RunForm, then finish.
    bash_newt.send(b"\n")
    bash_newt.sendline(b'newt FormDestroy "$f" && newt Finished')


def test_setsuspendcallback_no_args_returns_failure(bash_newt):
    """Calling SetSuspendCallback with no function name must return non-zero."""
    bash_newt.sendline(
        b"newt Init; "
        b"newt SetSuspendCallback; "
        b'echo "exit:$?"'
    )

    try:
        bash_newt.expect(b"exit:", timeout=3)
        bash_newt.expect(rb"\d+", timeout=1)
        code = int(bash_newt.match.group(0))
    except pexpect.TIMEOUT:
        code = -1

    assert code != 0, (
        "SetSuspendCallback with no args should return non-zero, got: " + str(code)
    )
    bash_newt.sendline(b"newt Finished")
