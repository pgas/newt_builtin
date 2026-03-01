"""Functional tests for ``newt PushHelpLine`` and ``newt PopHelpLine``.

The help line is a single line at the very bottom of the screen.
PushHelpLine sets it; PopHelpLine restores the previous value.
"""

import time
from conftest import render, screen_rows, screen_text


def test_pushhelpline_visible(bash_newt):
    """PushHelpLine should display the given text at the bottom of the screen."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt PushHelpLine "HELP:press F1" && '
        b'newt OpenWindow 5 3 50 10 "HelpLine" && '
        b'newt -v btn Button 3 2 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$btn" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt PopWindow && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    # The help line should appear on one of the bottom rows of the screen
    assert any("HELP:press F1" in r for r in rows), \
        f"PushHelpLine text not found on screen.\n{full}"

    bash_newt.send(b"\n")


def test_pophelpline_restores_previous(bash_newt):
    """PopHelpLine should restore the previous help line text."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt PushHelpLine "ORIGINAL_HELP" && '
        b'newt PushHelpLine "OVERRIDE_HELP" && '
        b'newt PopHelpLine && '
        b'newt OpenWindow 5 3 50 10 "PopHelp" && '
        b'newt -v btn Button 3 2 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$btn" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt PopWindow && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    # After PopHelpLine, "ORIGINAL_HELP" should be visible again
    assert any("ORIGINAL_HELP" in r for r in rows), \
        f"PopHelpLine did not restore original help line.\n{full}"
    assert not any("OVERRIDE_HELP" in r for r in rows), \
        f"Override help line still visible after PopHelpLine.\n{full}"

    bash_newt.send(b"\n")
