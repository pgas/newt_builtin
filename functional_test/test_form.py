"""Functional tests for form-related newt sub-commands.

Covers:
    FormRun       – hand-written wrapper that exposes newtExitStruct fields
    FormWatchFd   – registers a file-descriptor watch on a running form
    FormSetTimer  – sets a millisecond timer that fires while the form runs
    DrawForm      – renders the form without entering the event loop
    ComponentGetPosition – queries a component's absolute screen position
    ComponentGetSize     – queries a component's screen dimensions
    ComponentAddCallback – registers a bash function as a component callback
"""

import time

from conftest import render, screen_rows, screen_text


# ─── FormRun: timer exit ──────────────────────────────────────────────────────

def test_formrun_timer_exit(bash_newt):
    """FormRun should set reasonVar to TIMER when FormSetTimer fires."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 5 40 6 "FormRun Timer" && '
        b'newt -v lbl Label 3 1 "waiting..." && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$lbl" && '
        b'newt FormSetTimer "$f" 200 && '       # exit after 200 ms
        b'newt FormRun "$f" REASON VALUE && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "REASON=$REASON"'
    )
    # Give the 200 ms timer time to fire, then collect shell output.
    time.sleep(1.2)
    screen = render(bash_newt, initial_timeout=1.0, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("REASON=TIMER" in r for r in rows), (
        f"Expected 'REASON=TIMER' in output.\n{full}"
    )


# ─── FormRun: component (button) exit ────────────────────────────────────────

def test_formrun_component_exit(bash_newt):
    """FormRun should set reasonVar to COMPONENT when a button is activated."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 5 40 6 "FormRun Component" && '
        b'newt -v btn Button 3 1 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$btn" && '
        b'newt FormRun "$f" REASON VALUE && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "REASON=$REASON" && echo "VALUE=$VALUE"'
    )
    # Phase 1: wait for the form to render (it is blocking in FormRun).
    screen = render(bash_newt, initial_timeout=2.0)
    assert any("FormRun Component" in r for r in screen_rows(screen)), \
        f"Form window did not appear.\n{screen_text(screen)}"
    # Phase 2: press CR to activate the focused button, then capture output.
    bash_newt.send(b"\r")
    time.sleep(0.8)
    screen = render(bash_newt, initial_timeout=1.0, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("REASON=COMPONENT" in r for r in rows), (
        f"Expected 'REASON=COMPONENT' after pressing Enter.\n{full}"
    )
    assert any("VALUE=" in r for r in rows), (
        f"Expected VALUE= binding in output.\n{full}"
    )


# ─── FormWatchFd: fd watch + timer fallback ───────────────────────────────────

def test_formwatchfd_with_timer_fallback(bash_newt):
    """FormWatchFd should not crash; form exits via timer when fd is not ready."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 5 40 6 "WatchFd Test" && '
        b'newt -v lbl Label 3 1 "watch fd" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$lbl" && '
        # Watch a non-ready pipe read-end for readability.
        b'exec 9< <(sleep 10) && '
        b'newt FormWatchFd "$f" 9 1 && '        # 1 = NEWT_FD_READ
        b'newt FormSetTimer "$f" 300 && '       # timer will fire first
        b'newt FormRun "$f" REASON VALUE && '
        b'newt FormDestroy "$f" && '
        b'exec 9<&- && '
        b'newt Finished && '
        b'echo "REASON=$REASON"'
    )
    time.sleep(1.5)
    screen = render(bash_newt, initial_timeout=1.0, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    # The form should exit cleanly (TIMER or FDREADY—both are acceptable).
    assert any(("REASON=TIMER" in r or "REASON=FDREADY" in r) for r in rows), (
        f"Expected REASON=TIMER or REASON=FDREADY.\n{full}"
    )


# ─── DrawForm: renders without entering event loop ───────────────────────────

def test_drawform_renders_content(bash_newt):
    """DrawForm should render visible components without blocking."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 4 40 8 "DrawForm Test" && '
        b'newt -v lbl Label 3 1 "drawn label" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$lbl" && '
        b'newt DrawForm "$f" && '
        b'newt Refresh && '
        b'newt WaitForKey && '
        b'newt FormDestroy "$f" && '
        b'newt Finished'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("drawn label" in r for r in rows), (
        f"'drawn label' not visible after DrawForm.\n{full}"
    )
    bash_newt.send(b"\n")


# ─── ComponentGetPosition: absolute screen coords ────────────────────────────

def test_componentgetposition_returns_coords(bash_newt):
    """ComponentGetPosition should bind non-negative column and row."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "GetPosition" && '
        b'newt -v lbl Label 4 2 "pos label" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$lbl" && '
        b'newt DrawForm "$f" && '
        b'newt ComponentGetPosition "$lbl" LCOL LROW && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "LCOL=$LCOL LROW=$LROW"'
    )
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    # We expect "LCOL=<n> LROW=<n>" to appear somewhere on the screen.
    line = next((r for r in rows if r.startswith("LCOL=")), None)
    assert line is not None, (
        f"Expected 'LCOL=...' in output.\n{full}"
    )
    # Parse and do a sanity check – positions must be non-negative integers.
    parts = dict(p.split("=", 1) for p in line.split() if "=" in p)
    assert int(parts.get("LCOL", -1)) >= 0, (
        f"LCOL is negative or missing: {line}"
    )
    assert int(parts.get("LROW", -1)) >= 0, (
        f"LROW is negative or missing: {line}"
    )


# ─── ComponentGetSize: returns dimension values ───────────────────────────────

def test_componentgetsize_returns_dimensions(bash_newt):
    """ComponentGetSize should bind positive width and height."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "GetSize" && '
        b'newt -v btn Button 4 2 "ClickMe" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$btn" && '
        b'newt DrawForm "$f" && '
        b'newt ComponentGetSize "$btn" BWIDTH BHEIGHT && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "BWIDTH=$BWIDTH BHEIGHT=$BHEIGHT"'
    )
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    line = next((r for r in rows if r.startswith("BWIDTH=")), None)
    assert line is not None, (
        f"Expected 'BWIDTH=...' in output.\n{full}"
    )
    parts = dict(p.split("=", 1) for p in line.split() if "=" in p)
    assert int(parts.get("BWIDTH", 0)) > 0, (
        f"Expected positive BWIDTH: {line}"
    )
    assert int(parts.get("BHEIGHT", 0)) > 0, (
        f"Expected positive BHEIGHT: {line}"
    )


# ─── ComponentAddCallback: fires bash function on component event ─────────────

def test_componentaddcallback_accepts_args(bash_newt):
    """ComponentAddCallback should accept (co, bashExpr [data]) without crashing."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 5 40 6 "Callback Test" && '
        b'newt -v btn Button 3 1 "Fire" && '
        b"newt ComponentAddCallback \"$btn\" 'CB_FIRED=1' mydata ; "
        b'r=$? && '
        b'newt Finished && '
        b'echo "ACCEPT=$r"'
    )
    time.sleep(0.8)
    screen = render(bash_newt, initial_timeout=1.0, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("ACCEPT=0" in r for r in rows), (
        f"Expected 'ACCEPT=0' (argument parsing succeeded).\n{full}"
    )


def test_componentaddcallback_fires(bash_newt):
    """Callback expression should execute when a checkbox value changes."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 5 40 10 "Callback Fires" && '
        b'newt -v cb Checkbox 3 2 "Toggle me" && '
        b"newt ComponentAddCallback \"$cb\" 'CB_FIRED=1' && "
        b'newt -v _ok Button 3 5 "OK" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$cb" "$_ok" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "CB_FIRED=${CB_FIRED:-0}"'
    )
    render(bash_newt, initial_timeout=2.0)   # wait for form to appear
    bash_newt.send(b" ")    # Space: toggle checkbox → fires callback → CB_FIRED=1
    time.sleep(0.1)
    bash_newt.send(b"\t")   # Tab: move focus to OK button
    time.sleep(0.1)
    bash_newt.send(b"\r")   # Enter: click OK → exit form
    time.sleep(0.5)
    screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("CB_FIRED=1" in r for r in rows), (
        f"Expected 'CB_FIRED=1' after checkbox toggle.\n{full}"
    )


# ─── FormAddHotKey: hotkey exit ───────────────────────────────────────────────

def test_formaddhotkey_f12_exit(bash_newt):
    """FormAddHotKey should cause FormRun to exit with HOTKEY when the key is pressed."""
    # F12 key code = NEWT_KEY_F12 = NEWT_KEY_EXTRA_BASE + 24 = 0x8000 + 24 = 32792
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 10 5 40 6 "HotKey Test" && '
        b'newt -v lbl Label 3 1 "Press F12" && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$lbl" && '
        b'newt FormAddHotKey "$f" 32792 && '
        b'newt FormRun "$f" REASON VALUE && '
        b'newt FormDestroy "$f" && '
        b'newt Finished && '
        b'echo "REASON=$REASON VALUE=$VALUE"'
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Press F12" in r for r in rows), \
        f"Form window did not appear.\n{full}"

    # Send F12 (ESC [ 2 4 ~)
    bash_newt.send(b"\x1b[24~")
    time.sleep(0.8)
    screen = render(bash_newt, initial_timeout=1.0, drain_timeout=0.3)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("REASON=HOTKEY" in r for r in rows), \
        f"Expected 'REASON=HOTKEY' after pressing F12.\n{full}"
