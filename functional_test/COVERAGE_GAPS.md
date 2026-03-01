# Functional Test Coverage Gaps

Audit performed against the dispatch table in `src/newt_wrappers.cpp`
and all `functional_test/test_*.py` files.

---

## Summary

- **Total dispatch entries**: 120 (including aliases)
- **Functionally tested**: ~92 (after two coverage expansion sessions)
- **Remaining untested**: ~28

See *Suggested test expansions* below for current status of each priority item.

---

## Bugs fixed during audit

### 1. Entry tests passed extra bogus arguments (test_entry.py)

All `Entry` constructor calls passed 7 positional arguments
(`Entry 3 2 "hello" 30 0 "" 0`) to a wrapper that expects at most 5
(`Entry left top initialValue width [flags]`).  The extra `0 "" 0`
were silently ignored.  **Fixed**: removed the extra arguments.

### 2. `test_entry_set_flags_disabled` used wrong flag values (test_entry.py)

The test called `EntrySetFlags "$e" 2 1` (flag=2=HIDDEN, sense=1=RESET)
instead of `EntrySetFlags "$e" 8 0` (flag=8=DISABLED, sense=0=SET).
It only asserted text visibility (trivially true), never verifying the
entry was actually disabled.  **Fixed**: uses correct flags and verifies
that typing into the entry does not change its value.

### 3. `test_listbox_getcurrent` — use-after-free (test_listbox.py)

Called `ListboxGetCurrent "$lb"` *after* `FormDestroy "$f"`, accessing
freed component memory.  Worked by coincidence.  **Fixed**: query is now
issued before `FormDestroy` in the `&&` chain.

### 4. `test_radio_get_current_returns_value` — use-after-free (test_radiobutton.py)

Called `RadioGetCurrent "$r1"` *after* `FormDestroy "$f"`.
**Fixed**: same approach — query before destroy.

### 5. `test_radio_set_current` — use-after-free (test_radiobutton.py)

Called `RadioGetCurrent "$r1"` *after* `FormDestroy "$f"`.
**Fixed**: same approach — query before destroy.

---

## Subcommands with NO functional test

### Global / lifecycle (low priority — hard to observe visually)

| Subcommand | Notes |
|---|---|
| `ClearKeyBuffer` | No visible effect to assert on |
| `Bell` | Produces a terminal bell; hard to assert |
| `CursorOff` | Cursor visibility; hard to assert via pyte |
| `CursorOn` | Cursor visibility; hard to assert via pyte |
| `Suspend` | Only tested indirectly inside suspend callback shim |
| `Resume` | Only tested indirectly inside suspend callback shim |
| `Delay` | Blocks for N microseconds; could test timing |
| `ResizeScreen` | Needs pty resize; possible with pexpect `setwinsize()` |

### Window management

| Subcommand | Notes |
|---|---|
| `CenteredWindow` | ✅ `test_window.py::test_centeredwindow_title_visible` |
| `PopWindow` | ✅ `test_window.py::test_popwindow_removes_top_window` |
| `PopWindowNoRefresh` | No functional test — deferred refresh, hard to distinguish from PopWindow visually |
| `PushHelpLine` | ✅ `test_helpline.py::test_pushhelpline_visible` |
| `PopHelpLine` | ✅ `test_helpline.py::test_pophelpline_restores_previous` |
| `RedrawHelpLine` | No functional test — no observable state change beyond redraw |

### Widget operations

| Subcommand | Notes |
|---|---|
| `SetColor` | ✅ `test_setcolors.py::test_setcolor_single_pair` |
| `SetHelpCallback` | No functional test — registers a callback but triggers only on F1 context help |
| `LabelSetColors` | No functional test — color changes not observable via pyte |
| `VerticalScrollbar` | No functional test |
| `ScrollbarSet` | No functional test |
| `ScrollbarSetColors` | No functional test — color changes not observable via pyte |
| `ListboxSetWidth` | No functional test |
| `ListboxSetCurrentByKey` | ✅ `test_listbox.py::test_listbox_setcurrentbykey` |
| `ListboxSetData` | No functional test |
| `ListboxInsertEntry` | ✅ `test_listbox.py::test_listbox_insertentry_preserves_order` |
| `ListboxDeleteEntry` | ✅ `test_listbox.py::test_listbox_deleteentry` |
| `ListboxGetEntry` | ✅ `test_listbox.py::test_listbox_getentry` |
| `ListboxClearSelection` | No functional test |
| `TextboxSetHeight` | No functional test |
| `TextboxSetColors` | No functional test — color changes not observable via pyte |
| `CheckboxTreeSetEntry` | No functional test |
| `CheckboxTreeSetWidth` | No functional test |
| `Scale` (constructor) | ✅ `test_scale.py::test_scale_visible` |
| `ScaleSet` | ✅ `test_scale.py::test_scale_set_full` |
| `ScaleSetColors` | No functional test — color changes not observable via pyte |

### Entry operations

| Subcommand | Notes |
|---|---|
| `EntrySetColors` | No functional test — color changes not observable via pyte |
| `EntryGetCursorPosition` | No functional test |
| `EntrySetCursorPosition` | No functional test |
| `EntrySetFilter` | ✅ `test_entry.py::test_entry_set_filter_blocks_digits` |

### Form operations

| Subcommand | Notes |
|---|---|
| `FormSetSize` | No functional test |
| `FormGetCurrent` | ✅ `test_form.py::test_form_setcurrent_getcurrent` |
| `FormSetBackground` | No functional test — color changes not observable via pyte |
| `FormSetCurrent` | ✅ `test_form.py::test_form_setcurrent_getcurrent` |
| `FormSetHeight` | No functional test |
| `FormSetWidth` | No functional test |
| `FormAddHotKey` | ✅ `test_form.py::test_formaddhotkey_f12_exit` |
| `FormGetScrollPosition` | No functional test |
| `FormSetScrollPosition` | No functional test |

### Component operations

| Subcommand | Notes |
|---|---|
| `ComponentDestroy` | ✅ `test_component.py::test_component_destroy_standalone` |
| `ComponentTakesFocus` | ✅ `test_component.py::test_component_takes_focus_skip` |
| `ComponentAddDestroyCallback` | ✅ `test_component.py::test_component_add_destroy_callback_fires` |
| `GetScreenSize` | ✅ `test_getscreensize.py::test_getscreensize_returns_positive` |

### Grid operations

| Subcommand | Notes |
|---|---|
| `GridSimpleWindow` | ✅ `test_grid.py::test_gridsimplewindow` |
| `GridDestroy` | No functional test — alias for GridFree |

---

## Weak / shallow assertions — status

| Test | Original Weakness | Status |
|---|---|---|
| `test_checkbox_set_flags_disable` | Only checks label visible, not that checkbox is non-interactive | Still weak (hard to verify interactivity via pty) |
| `test_listbox_setcurrent` | Doesn't verify which item is highlighted | Still weak (highlight color not observable via pyte) |
| `test_checkboxtree_getentryvalue` | Asserted `val=[` not the specific value `*` | ✅ Fixed: now asserts `val=[*]` |
| `test_textbox_get_num_lines` | Asserted `n=[` not the exact count 3 | ✅ Fixed: now asserts `n=[3]` |
| `test_checkboxtree_setcurrent_getcurrent` | Only asserted non-empty, not expected data key | ✅ Fixed: now asserts `cur=[2]` |
| `test_reflow_text` | Asserted non-zero width/height | ✅ Strengthened: asserts `rh >= 3` for 7-word text |

---

## Suggested test expansions (priority order)

### High priority — all done ✅

1. ✅ **`GetScreenSize`** — `test_getscreensize.py`
2. ✅ **`CenteredWindow`** — `test_window.py`
3. ✅ **`PopWindow`** — `test_window.py`
4. ✅ **`PushHelpLine` / `PopHelpLine`** — `test_helpline.py`
5. ✅ **`Scale` / `ScaleSet`** — `test_scale.py`
6. ✅ **`FormAddHotKey`** — `test_form.py::test_formaddhotkey_f12_exit` (verifies REASON=HOTKEY and VALUE=32880)
7. ✅ **`EntrySetFilter`** — `test_entry.py::test_entry_set_filter_blocks_digits`
8. ✅ **`ComponentAddDestroyCallback`** — `test_component.py`
9. ✅ **`ComponentTakesFocus`** — `test_component.py`
10. ✅ **`ComponentDestroy`** — `test_component.py`

### Medium priority — all done ✅

11. ✅ **`ListboxGetEntry`** — `test_listbox.py::test_listbox_getentry`
12. ✅ **`ListboxDeleteEntry`** — `test_listbox.py::test_listbox_deleteentry`
13. ✅ **`ListboxInsertEntry`** — `test_listbox.py::test_listbox_insertentry_preserves_order`
14. ✅ **`ListboxSetCurrentByKey`** — `test_listbox.py::test_listbox_setcurrentbykey`
15. ✅ **`FormGetCurrent` / `FormSetCurrent`** — `test_form.py::test_form_setcurrent_getcurrent`
16. ✅ **`FormAddHotKey` + `FormRun`** — covered by item 6 above; VALUE verified
17. ✅ **`GridSimpleWindow`** — `test_grid.py::test_gridsimplewindow`
18. ✅ **`SetColor`** — `test_setcolors.py::test_setcolor_single_pair`
19. ✅ **`ReflowText`** — asserting `rh >= 3` and exact line count

### Low priority — hard to test or low-risk

20. **`ClearKeyBuffer`** — no visible effect
21. **`Bell`** — terminal bell
22. **`CursorOff` / `CursorOn`** — cursor state
23. **`Suspend` / `Resume`** — tested indirectly via SetSuspendCallback
24. **`Delay`** — timing-based
25. **`ResizeScreen`** — needs pty resize
26. **Color-setting operations** (`LabelSetColors`, `TextboxSetColors`, `EntrySetColors`, `ScaleSetColors`, `ScrollbarSetColors`) — verify no crash, hard to assert on actual color

---

## Implementation notes

### CheckboxGetValue workaround (documented, legitimate)

`wrap_CheckboxGetValue` reads from `g_checkbox_results` (heap-allocated
result pointer) instead of calling `newtCheckboxGetValue()` directly.
The comment explains that `newtCheckboxGetValue()` can return a stale
value because it reads `cb->currValue` which is not always updated by
the draw callback.  This is a correct workaround for a known libnewt
behavior, not a hidden bug.

### Extra arguments warning — implemented

`call_newt`-based wrappers now emit a `stderr` warning when extra trailing
arguments are detected.  The check was added to both the void-return and
non-void-return overloads in `src/newt_arg_parser.hpp`.
