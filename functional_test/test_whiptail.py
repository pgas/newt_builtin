"""Functional tests for whiptail.sh — the bash-builtin whiptail replacement."""

import pathlib
import time
import pexpect
import pyte
import pytest

WHIPTAIL_SH = str(
    pathlib.Path(__file__).parent.parent / "whiptail.sh"
)
COLS, ROWS = 80, 24


# ---------------------------------------------------------------------------
# Helpers (duplicated from conftest to keep this self-contained)
# ---------------------------------------------------------------------------

def drain(child, initial_timeout=2.0, drain_timeout=0.3):
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


def render(child, **kwargs):
    screen = pyte.Screen(COLS, ROWS)
    stream = pyte.ByteStream(screen)
    stream.feed(drain(child, **kwargs))
    return screen


def screen_rows(screen):
    return [
        "".join(screen.buffer[row][col].data for col in range(screen.columns)).rstrip()
        for row in range(screen.lines)
    ]


def screen_text(screen):
    return "\n".join(screen_rows(screen))


def spawn_bash():
    """Spawn a fresh bash session in a pty."""
    child = pexpect.spawn(
        "bash",
        ["--norc", "--noprofile"],
        encoding=None,
        dimensions=(ROWS, COLS),
        timeout=10,
    )
    # Give bash a moment to start
    try:
        child.expect(pexpect.TIMEOUT, timeout=0.5)
    except pexpect.EOF:
        pytest.fail("bash exited unexpectedly")
    return child


# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

class TestMsgbox:
    def test_msgbox_displays(self):
        child = spawn_bash()
        try:
            child.sendline(f'{WHIPTAIL_SH} --title "Hello" --msgbox "Test message" 10 40'.encode())
            screen = render(child, initial_timeout=3.0)
            rows = screen_rows(screen)
            full = screen_text(screen)

            assert any("Hello" in r for r in rows), f"Title not visible.\n{full}"
            assert any("Test message" in r for r in rows), f"Text not visible.\n{full}"
            assert any("Ok" in r for r in rows), f"Ok button not visible.\n{full}"

            # Press Enter to dismiss
            child.send(b"\r")
            time.sleep(0.5)
        finally:
            child.close(force=True)


class TestYesno:
    def test_yesno_displays(self):
        child = spawn_bash()
        try:
            child.sendline(f'{WHIPTAIL_SH} --title "Confirm" --yesno "Continue?" 8 40'.encode())
            screen = render(child, initial_timeout=3.0)
            rows = screen_rows(screen)
            full = screen_text(screen)

            assert any("Confirm" in r for r in rows), f"Title not visible.\n{full}"
            assert any("Continue?" in r for r in rows), f"Text not visible.\n{full}"
            assert any("Yes" in r for r in rows), f"Yes button not visible.\n{full}"

            # Press Enter (Yes)
            child.send(b"\r")
            time.sleep(0.5)
            screen = render(child, initial_timeout=1.5, drain_timeout=0.3)
        finally:
            child.close(force=True)

    def test_yesno_yes_returns_0(self):
        child = spawn_bash()
        try:
            child.sendline(
                f'{WHIPTAIL_SH} --title "Q" --yesno "Yes?" 8 30; echo "rc=$?"'.encode()
            )
            screen = render(child, initial_timeout=3.0)
            # Press Enter to accept Yes
            child.send(b"\r")
            time.sleep(0.5)
            screen = render(child, initial_timeout=2.0)
            rows = screen_rows(screen)
            full = screen_text(screen)
            assert any("rc=0" in r for r in rows), f"Expected rc=0.\n{full}"
        finally:
            child.close(force=True)

    def test_yesno_no_returns_1(self):
        child = spawn_bash()
        try:
            child.sendline(
                f'{WHIPTAIL_SH} --title "Q" --yesno "No?" 8 30; echo "rc=$?"'.encode()
            )
            screen = render(child, initial_timeout=3.0)
            # Tab to No button, then Enter
            child.send(b"\t")
            time.sleep(0.3)
            child.send(b"\r")
            time.sleep(0.5)
            screen = render(child, initial_timeout=2.0)
            rows = screen_rows(screen)
            full = screen_text(screen)
            assert any("rc=1" in r for r in rows), f"Expected rc=1.\n{full}"
        finally:
            child.close(force=True)


class TestInputbox:
    def test_inputbox_returns_value(self):
        child = spawn_bash()
        try:
            child.sendline(
                f'exec 3>&1; R=$({WHIPTAIL_SH} --title "In" --inputbox "Name:" 8 40 "Alice" 2>&1 1>&3); exec 3>&-; echo "val=[$R]"'.encode()
            )
            screen = render(child, initial_timeout=3.0)
            rows = screen_rows(screen)
            full = screen_text(screen)
            assert any("Name:" in r for r in rows), f"Text not visible.\n{full}"

            # Accept the default value "Alice"
            child.send(b"\r")
            time.sleep(0.5)
            screen = render(child, initial_timeout=2.0)
            rows = screen_rows(screen)
            full = screen_text(screen)
            assert any("val=[Alice]" in r for r in rows), f"Expected val=[Alice].\n{full}"
        finally:
            child.close(force=True)


class TestMenu:
    def test_menu_returns_tag(self):
        child = spawn_bash()
        try:
            child.sendline(
                f'exec 3>&1; R=$({WHIPTAIL_SH} --title "M" --menu "Pick:" 14 40 3 '
                f'"a" "Alpha" "b" "Beta" "c" "Gamma" 2>&1 1>&3); exec 3>&-; echo "val=[$R]"'.encode()
            )
            screen = render(child, initial_timeout=3.0)
            rows = screen_rows(screen)
            full = screen_text(screen)
            assert any("Pick:" in r for r in rows), f"Text not visible.\n{full}"

            # Press Enter to select the first item ("a")
            child.send(b"\r")
            time.sleep(0.5)
            screen = render(child, initial_timeout=2.0)
            rows = screen_rows(screen)
            full = screen_text(screen)
            assert any("val=[a]" in r for r in rows), f"Expected val=[a].\n{full}"
        finally:
            child.close(force=True)


class TestInfobox:
    def test_infobox_displays_and_exits(self):
        child = spawn_bash()
        try:
            child.sendline(
                f'{WHIPTAIL_SH} --title "Info" --infobox "Working..." 7 30; echo "done"'.encode()
            )
            time.sleep(1.5)
            screen = render(child, initial_timeout=2.0)
            rows = screen_rows(screen)
            full = screen_text(screen)
            # Infobox exits immediately, so "done" should appear
            assert any("done" in r for r in rows), f"Expected 'done' after infobox.\n{full}"
        finally:
            child.close(force=True)


class TestGauge:
    def test_gauge_runs_and_exits(self):
        child = spawn_bash()
        try:
            child.sendline(
                f'printf "25\\n50\\n75\\n100\\n" | {WHIPTAIL_SH} --title "Prog" --gauge "Loading" 7 40 0; echo "gauge_done"'.encode()
            )
            time.sleep(2.0)
            screen = render(child, initial_timeout=2.0)
            rows = screen_rows(screen)
            full = screen_text(screen)
            assert any("gauge_done" in r for r in rows), f"Expected gauge_done.\n{full}"
        finally:
            child.close(force=True)
