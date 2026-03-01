"""Functional tests for grid layout functions.

Covers: CreateGrid, GridSetField, GridPlace, GridGetSize, GridWrappedWindow,
GridWrappedWindowAt, GridBasicWindow, GridAddComponentsToForm, ButtonBar,
GridVStacked, GridHStacked, GridHCloseStacked, GridVCloseStacked.
"""

import time
from conftest import render, screen_rows, screen_text


def test_grid_wrapped_window(bash_newt):
    """GridWrappedWindow should create a window around the grid content."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v tb Textbox 0 0 30 3 0 && '
        b'newt TextboxSetText "$tb" "Grid Window Text" && '
        b'newt -v b Button 0 0 "Close" && '
        b'newt -v btns GridVStacked 1 "$b" && '
        b'newt -v grid GridBasicWindow "$tb" "$btns" "$btns" && '
        b'newt GridWrappedWindow "$grid" "Grid Title" && '
        b'newt -v f Form "" "" 0 && '
        b'newt GridAddComponentsToForm "$grid" "$f" 1 && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt GridFree "$grid" 1 && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Grid Title" in r for r in rows), \
        f"GridWrappedWindow title not found.\n{full}"
    assert any("Grid Window Text" in r for r in rows), \
        f"Grid text content not visible.\n{full}"

    bash_newt.send(b"\n")


def test_create_grid_setfield_place(bash_newt):
    """CreateGrid + GridSetField + GridPlace should position a button."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "Grid Place" && '
        b'newt -v b Button 0 0 "GridBtn" && '
        b'newt -v g CreateGrid 1 1 && '
        b'newt GridSetField "$g" 0 0 1 "$b" 1 1 1 1 0 0 && '
        b'newt GridPlace "$g" 5 2 && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponent "$f" "$b" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt GridFree "$g" 0 && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("GridBtn" in r for r in rows), \
        f"Button placed via GridSetField not visible.\n{full}"

    bash_newt.send(b"\n")


def test_grid_get_size(bash_newt):
    """GridGetSize should bind non-zero width and height."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v b Button 0 0 "GSize" && '
        b'newt -v g CreateGrid 1 1 && '
        b'newt GridSetField "$g" 0 0 1 "$b" 0 0 0 0 0 0 && '
        b'newt GridGetSize "$g" gw gh && '
        b'newt Finished && '
        b'echo "gw=[$gw] gh=[$gh]"'
    )
    time.sleep(0.3)
    screen = render(bash_newt, initial_timeout=2.0, drain_timeout=0.5)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("gw=[" in r for r in rows), \
        f"GridGetSize widthVar not bound.\n{full}"
    assert any("gh=[" in r for r in rows), \
        f"GridGetSize heightVar not bound.\n{full}"


def test_button_bar_visible(bash_newt):
    """ButtonBar should create a grid of labelled buttons."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 60 10 "ButtonBar Test" && '
        b'newt -v bbgrid ButtonBar "Save" b_save "Quit" b_quit && '
        b'newt GridPlace "$bbgrid" 5 2 && '
        b'newt -v f Form "" "" 0 && '
        b'newt FormAddComponents "$f" "$b_save" "$b_quit" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt GridFree "$bbgrid" 0 && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Save" in r for r in rows), \
        f"ButtonBar 'Save' button not visible.\n{full}"
    assert any("Quit" in r for r in rows), \
        f"ButtonBar 'Quit' button not visible.\n{full}"

    bash_newt.send(b"\n")


def test_grid_vstacked(bash_newt):
    """GridVStacked should stack widgets vertically."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v b1 Button 0 0 "Top" && '
        b'newt -v b2 Button 0 0 "Bot" && '
        b'newt -v vg GridVStacked 1 "$b1" 1 "$b2" && '
        b'newt GridWrappedWindow "$vg" "VStack" && '
        b'newt -v f Form "" "" 0 && '
        b'newt GridAddComponentsToForm "$vg" "$f" 1 && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt GridFree "$vg" 1 && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Top" in r for r in rows), \
        f"GridVStacked first button not visible.\n{full}"
    assert any("Bot" in r for r in rows), \
        f"GridVStacked second button not visible.\n{full}"

    bash_newt.send(b"\n")


def test_grid_wrapped_window_at(bash_newt):
    """GridWrappedWindowAt should create a window at an explicit position."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v b Button 0 0 "AtBtn" && '
        b'newt -v g GridVStacked 1 "$b" && '
        b'newt GridWrappedWindowAt "$g" "At Title" 5 3 && '
        b'newt -v f Form "" "" 0 && '
        b'newt GridAddComponentsToForm "$g" "$f" 1 && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt GridFree "$g" 1 && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("At Title" in r for r in rows), \
        f"GridWrappedWindowAt title not visible.\n{full}"
    assert any("AtBtn" in r for r in rows), \
        f"GridWrappedWindowAt button not visible.\n{full}"

    bash_newt.send(b"\n")


def test_grid_hstacked(bash_newt):
    """GridHStacked should place two buttons side by side."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v b1 Button 0 0 "Left" && '
        b'newt -v b2 Button 0 0 "Right" && '
        b'newt -v hg GridHStacked 1 "$b1" 1 "$b2" && '
        b'newt GridWrappedWindow "$hg" "HStack" && '
        b'newt -v f Form "" "" 0 && '
        b'newt GridAddComponentsToForm "$hg" "$f" 1 && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt GridFree "$hg" 1 && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Left" in r for r in rows), \
        f"GridHStacked first button not visible.\n{full}"
    assert any("Right" in r for r in rows), \
        f"GridHStacked second button not visible.\n{full}"

    bash_newt.send(b"\n")


def test_grid_hclose_and_vclose_stacked(bash_newt):
    """GridHCloseStacked and GridVCloseStacked should render buttons."""
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v ba Button 0 0 "Ah" && '
        b'newt -v bb Button 0 0 "Bh" && '
        b'newt -v hcg GridHCloseStacked 1 "$ba" 1 "$bb" && '
        b'newt -v bc Button 0 0 "Av" && '
        b'newt -v bd Button 0 0 "Bv" && '
        b'newt -v vcg GridVCloseStacked 1 "$bc" 1 "$bd" && '
        b'newt -v outer GridVStacked 2 "$hcg" 2 "$vcg" && '
        b'newt GridWrappedWindow "$outer" "Close" && '
        b'newt -v f Form "" "" 0 && '
        b'newt GridAddComponentsToForm "$outer" "$f" 1 && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt GridFree "$outer" 1 && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Ah" in r for r in rows), \
        f"GridHCloseStacked first button not visible.\n{full}"
    assert any("Av" in r for r in rows), \
        f"GridVCloseStacked first button not visible.\n{full}"

    bash_newt.send(b"\n")


# ─── GridSimpleWindow ─────────────────────────────────────────────────────────

def test_gridsimplewindow(bash_newt):
    """GridSimpleWindow should create a wrapped window with text + buttons."""
    # GridSimpleWindow(text: newtComponent, middle: newtComponent, buttons: newtGrid)
    # Unlike GridBasicWindow, the middle slot is a single component, not a grid.
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt -v tb Textbox 0 0 30 2 0 && '
        b'newt TextboxSetText "$tb" "SimpleWindow text" && '
        b'newt -v lbl Label 0 0 "mid-item" && '
        b'newt -v b Button 0 0 "Close" && '
        b'newt -v btns GridVStacked 1 "$b" && '
        b'newt -v grid GridSimpleWindow "$tb" "$lbl" "$btns" && '
        b'newt GridWrappedWindow "$grid" "Simple Title" && '
        b'newt -v f Form "" "" 0 && '
        b'newt GridAddComponentsToForm "$grid" "$f" 1 && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b'newt GridFree "$grid" 1 && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Simple Title" in r for r in rows), \
        f"GridSimpleWindow title not found.\n{full}"
    assert any("mid-item" in r for r in rows), \
        f"GridSimpleWindow middle component not visible.\n{full}"

    bash_newt.send(b"\n")
