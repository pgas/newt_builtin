"""Functional tests for the ``newt Win*`` convenience dialog wrappers.

Covers: WinMessage, WinChoice, WinTernary, WinMenu.
These open self-contained windowed dialogs; tests verify the dialog is rendered
on screen and that return values are captured correctly after dismissal.
"""

import time
from conftest import render, screen_rows, screen_text


def test_winmessage_visible(bash_newt):
    """WinMessage should display the title, text, and button."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt WinMessage "Alert Title" "Dismiss" "This is the message body" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Alert Title" in r for r in rows), \
        f"WinMessage title not found.\n{full}"
    assert any("message body" in r for r in rows), \
        f"WinMessage body text not found.\n{full}"
    assert any("Dismiss" in r for r in rows), \
        f"WinMessage button text not found.\n{full}"

    # Dismiss the dialog.
    bash_newt.send(b"\r")


def test_winchoice_visible(bash_newt):
    """WinChoice should display both buttons and the question text."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v rc WinChoice "Confirm" "Yes" "No" "Delete this file?" && '
        b'newt Finished && '
        b'echo "rc=[$rc]"'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Confirm" in r for r in rows), \
        f"WinChoice window title not found.\n{full}"
    assert any("Yes" in r for r in rows), \
        f"WinChoice 'Yes' button not found.\n{full}"
    assert any("No" in r for r in rows), \
        f"WinChoice 'No' button not found.\n{full}"

    # Press Enter to select the focused button (button 1 = "Yes") → rc=1.
    bash_newt.send(b"\r")
    time.sleep(0.5)
    screen2 = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows2 = screen_rows(screen2)
    full2 = screen_text(screen2)

    assert any("rc=[1]" in r for r in rows2), \
        f"WinChoice should have returned 1 (yes); got:\n{full2}"


def test_winchoice_second_button(bash_newt):
    """Tab to the second button before pressing Enter should return 2."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v rc WinChoice "Pick" "Option1" "Option2" "Choose one" && '
        b'newt Finished && '
        b'echo "rc=[$rc]"'
    )
    # Wait for the dialog to appear, tab to button 2, then Enter.
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    assert any("Option1" in r or "Option2" in r for r in rows), \
        f"WinChoice buttons not visible.\n{screen_text(screen)}"

    bash_newt.send(b"\t")    # tab to button 2
    time.sleep(0.1)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    screen2 = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows2 = screen_rows(screen2)
    full2 = screen_text(screen2)

    assert any("rc=[2]" in r for r in rows2), \
        f"WinChoice should return 2 after tabbing; got:\n{full2}"


def test_winternary_visible(bash_newt):
    """WinTernary should display all three buttons."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v rc WinTernary "Save?" "Save" "Discard" "Cancel" "Unsaved changes" && '
        b'newt Finished && '
        b'echo "rc=[$rc]"'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Save" in r for r in rows), \
        f"WinTernary 'Save' button not found.\n{full}"
    assert any("Discard" in r for r in rows), \
        f"WinTernary 'Discard' button not found.\n{full}"
    assert any("Cancel" in r for r in rows), \
        f"WinTernary 'Cancel' button not found.\n{full}"

    # Press Enter → rc=1 (Save)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    screen2 = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    full2 = screen_text(screen2)
    rows2 = screen_rows(screen2)

    assert any("rc=[1]" in r for r in rows2), \
        f"WinTernary should return 1 (Save); got:\n{full2}"


def test_winmenu_items_visible(bash_newt):
    """WinMenu items should appear in the rendered dialog."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v rc WinMenu "Choose" "Select an item:" 50 3 3 6 '
        b'sel 3 "Alpha" "Beta" "Gamma" '
        b'"OK" && '
        b'newt Finished && '
        b'echo "rc=[$rc] sel=[$sel]"'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Alpha" in r for r in rows), \
        f"WinMenu item 'Alpha' not visible.\n{full}"
    assert any("Beta" in r for r in rows), \
        f"WinMenu item 'Beta' not visible.\n{full}"

    # Press Enter → rc=1, sel=0 (first item selected by default)
    bash_newt.send(b"\r")
    time.sleep(0.5)
    screen2 = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows2 = screen_rows(screen2)
    full2 = screen_text(screen2)

    assert any("rc=[1]" in r for r in rows2), \
        f"WinMenu should return rc=1; got:\n{full2}"
    assert any("sel=[" in r for r in rows2), \
        f"WinMenu should bind sel; got:\n{full2}"
