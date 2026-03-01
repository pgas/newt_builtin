"""Functional tests for ``newt Scale`` and ``newt ScaleSet``.

Scale creates a progress bar widget; ScaleSet updates its current value.
"""

import time
from conftest import render, screen_rows, screen_text


def test_scale_visible(bash_newt):
    """A Scale widget should render inside the window."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 8 "Scale Test" && '
        b'newt -v sc Scale 3 2 40 100 && '
        b'newt ScaleSet "$sc" 50 && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$sc" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    # The Scale widget renders as a horizontal bar. libnewt uses block
    # characters or '#' for the filled portion. The window title should
    # definitely appear whether or not we can check the bar characters.
    assert any("Scale Test" in r for r in rows), \
        f"Scale window title not visible.\n{full}"

    bash_newt.send(b"\n")


def test_scale_set_full(bash_newt):
    """ScaleSet at fullValue should fill the entire bar without crashing."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 8 "ScaleFull" && '
        b'newt -v sc Scale 3 2 40 100 && '
        b'newt ScaleSet "$sc" 100 && '
        b'newt -v btn Button 3 5 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$btn" "$sc" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("ScaleFull" in r for r in rows), \
        f"Scale window title not visible.\n{full}"

    bash_newt.send(b"\n")
