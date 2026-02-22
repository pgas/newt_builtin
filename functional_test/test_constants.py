"""Functional tests for NEWT_* constant arrays.

When the newt builtin is loaded with ``enable -f newt.so newt``, eight
read-only associative arrays are created automatically by
``register_newt_constants()``:

    NEWT_COLORSET   NEWT_FLAG  NEWT_FD   NEWT_ARG
    NEWT_CHECKBOXTREE  NEWT_KEY  NEWT_ANCHOR  NEWT_GRID_FLAG

These tests verify that:
  * Each array is declared as a read-only associative array (``-Ar``).
  * Spot-checked values match the libnewt ``#define`` constants.
  * Flags can be combined with bash arithmetic as expected.
  * Attempting to write to an array fails with a readonly error.
"""

import time

from conftest import render, screen_rows, screen_text


# ─── helpers ──────────────────────────────────────────────────────────────────

def _send_echo(bash_newt, expr: str, label: str) -> str:
    """Send ``echo "label=<expr>"`` and return all screen rows as text."""
    bash_newt.sendline(f'echo "{label}=$({expr})"'.encode())
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=0.8, drain_timeout=0.2)
    return screen_text(screen)


def _check_value(bash_newt, array: str, key: str, expected: int):
    """Assert that ``${array[key]}`` equals *expected*."""
    bash_newt.sendline(
        f'echo "CHECK_{array}_{key}=${{{array}[{key}]}}"'.encode()
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=0.8, drain_timeout=0.2)
    rows = screen_rows(screen)
    full = screen_text(screen)
    needle = f"CHECK_{array}_{key}={expected}"
    assert any(needle in r for r in rows), (
        f"Expected '{needle}' in output.\n{full}"
    )


# ─── array existence and attribute tests ──────────────────────────────────────

def test_all_arrays_are_declared_readonly_assoc(bash_newt):
    """declare -p for every array should show -Ar (readonly associative)."""
    arrays = [
        "NEWT_COLORSET", "NEWT_FLAG", "NEWT_FD", "NEWT_ARG",
        "NEWT_CHECKBOXTREE", "NEWT_KEY", "NEWT_ANCHOR", "NEWT_GRID_FLAG",
    ]
    for name in arrays:
        bash_newt.sendline(f'declare -p {name} | head -c 30'.encode())
        time.sleep(0.3)
        screen = render(bash_newt, initial_timeout=0.8, drain_timeout=0.2)
        rows = screen_rows(screen)
        full = screen_text(screen)
        assert any("-Ar" in r for r in rows), (
            f"Expected '{name}' to be a readonly associative array (-Ar).\n{full}"
        )


# ─── NEWT_COLORSET spot-checks ────────────────────────────────────────────────

def test_colorset_root(bash_newt):
    _check_value(bash_newt, "NEWT_COLORSET", "ROOT", 2)

def test_colorset_button(bash_newt):
    _check_value(bash_newt, "NEWT_COLORSET", "BUTTON", 7)

def test_colorset_actbutton(bash_newt):
    _check_value(bash_newt, "NEWT_COLORSET", "ACTBUTTON", 8)

def test_colorset_listbox(bash_newt):
    _check_value(bash_newt, "NEWT_COLORSET", "LISTBOX", 13)

def test_colorset_actlistbox(bash_newt):
    _check_value(bash_newt, "NEWT_COLORSET", "ACTLISTBOX", 14)


# ─── NEWT_FLAG spot-checks ────────────────────────────────────────────────────

def test_flag_returnexit(bash_newt):
    _check_value(bash_newt, "NEWT_FLAG", "RETURNEXIT", 1)

def test_flag_hidden(bash_newt):
    _check_value(bash_newt, "NEWT_FLAG", "HIDDEN", 2)

def test_flag_scroll(bash_newt):
    _check_value(bash_newt, "NEWT_FLAG", "SCROLL", 4)

def test_flag_disabled(bash_newt):
    _check_value(bash_newt, "NEWT_FLAG", "DISABLED", 8)

def test_flag_password(bash_newt):
    _check_value(bash_newt, "NEWT_FLAG", "PASSWORD", 2048)


# ─── NEWT_FLAG arithmetic: flags can be OR-combined ──────────────────────────

def test_flag_bitmask_combination(bash_newt):
    """SCROLL|RETURNEXIT should equal 5 (0x01 | 0x04)."""
    bash_newt.sendline(
        b'echo "COMBO=$(( ${NEWT_FLAG[SCROLL]} | ${NEWT_FLAG[RETURNEXIT]} ))"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=0.8, drain_timeout=0.2)
    rows = screen_rows(screen)
    full = screen_text(screen)
    assert any("COMBO=5" in r for r in rows), (
        f"Expected 'COMBO=5' (SCROLL|RETURNEXIT).\n{full}"
    )


# ─── NEWT_FD spot-checks ─────────────────────────────────────────────────────

def test_fd_read(bash_newt):
    _check_value(bash_newt, "NEWT_FD", "READ", 1)

def test_fd_write(bash_newt):
    _check_value(bash_newt, "NEWT_FD", "WRITE", 2)

def test_fd_except(bash_newt):
    _check_value(bash_newt, "NEWT_FD", "EXCEPT", 4)


# ─── NEWT_ARG spot-checks ─────────────────────────────────────────────────────

def test_arg_last(bash_newt):
    _check_value(bash_newt, "NEWT_ARG", "LAST", -100000)

def test_arg_append(bash_newt):
    _check_value(bash_newt, "NEWT_ARG", "APPEND", -1)


# ─── NEWT_CHECKBOXTREE spot-checks ───────────────────────────────────────────

def test_checkboxtree_unselected(bash_newt):
    """UNSELECTED is the ASCII value of space (32)."""
    _check_value(bash_newt, "NEWT_CHECKBOXTREE", "UNSELECTED", 32)

def test_checkboxtree_selected(bash_newt):
    """SELECTED is the ASCII value of '*' (42)."""
    _check_value(bash_newt, "NEWT_CHECKBOXTREE", "SELECTED", 42)

def test_checkboxtree_unselectable_flag(bash_newt):
    _check_value(bash_newt, "NEWT_CHECKBOXTREE", "UNSELECTABLE", 1 << 12)


# ─── NEWT_KEY spot-checks ─────────────────────────────────────────────────────

def test_key_enter(bash_newt):
    """ENTER is carriage return (13)."""
    _check_value(bash_newt, "NEWT_KEY", "ENTER", 13)

def test_key_escape(bash_newt):
    """ESCAPE is ESC (27)."""
    _check_value(bash_newt, "NEWT_KEY", "ESCAPE", 27)

def test_key_f1(bash_newt):
    """F1 = NEWT_KEY_EXTRA_BASE (0x8000 = 32768) + 101 = 32869."""
    _check_value(bash_newt, "NEWT_KEY", "F1", 0x8000 + 101)

def test_key_f12(bash_newt):
    _check_value(bash_newt, "NEWT_KEY", "F12", 0x8000 + 112)


# ─── NEWT_ANCHOR spot-checks ─────────────────────────────────────────────────

def test_anchor_left(bash_newt):
    _check_value(bash_newt, "NEWT_ANCHOR", "LEFT", 1)

def test_anchor_bottom(bash_newt):
    _check_value(bash_newt, "NEWT_ANCHOR", "BOTTOM", 8)


# ─── NEWT_GRID_FLAG spot-checks ──────────────────────────────────────────────

def test_grid_flag_growx(bash_newt):
    _check_value(bash_newt, "NEWT_GRID_FLAG", "GROWX", 1)

def test_grid_flag_growy(bash_newt):
    _check_value(bash_newt, "NEWT_GRID_FLAG", "GROWY", 2)


# ─── readonly enforcement ─────────────────────────────────────────────────────

def test_arrays_are_readonly(bash_newt):
    """Writing to a constant array should produce a 'readonly variable' error.

    Bash aborts the entire statement on a readonly violation, so we check for
    the error message rather than trying to echo $? on the same line.
    """
    bash_newt.sendline(b'NEWT_FLAG[SCROLL]=0')
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=0.8, drain_timeout=0.2)
    rows = screen_rows(screen)
    full = screen_text(screen)
    assert any("readonly" in r.lower() for r in rows), (
        f"Expected 'readonly variable' error message.\n{full}"
    )
