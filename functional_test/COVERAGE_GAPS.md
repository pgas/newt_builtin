# Functional Test Coverage Gaps

Audit performed against the dispatch table in `src/newt_wrappers.cpp`
and all `functional_test/test_*.py` files.

---

## Summary

- **Total dispatch entries**: 120 (including aliases)
- **Functionally tested**: 72
- **Not tested**: 48

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
| `CenteredWindow` | Like OpenWindow but auto-centered; easy to add |
| `PopWindow` | Closes the topmost window; test with two stacked windows |
| `PopWindowNoRefresh` | Same as PopWindow but defers refresh |
| `PushHelpLine` | Displays text on the help line at bottom |
| `PopHelpLine` | Restores the previous help line |
| `RedrawHelpLine` | Redraws the current help line |

### Widget operations

| Subcommand | Notes |
|---|---|
| `SetColor` | Sets a single colorset pair; distinct from full `SetColors` |
| `SetHelpCallback` | Registers a help callback function |
| `LabelSetColors` | Changes label colorset |
| `VerticalScrollbar` | Creates a standalone scrollbar widget |
| `ScrollbarSet` | Sets scrollbar position |
| `ScrollbarSetColors` | Changes scrollbar colors |
| `ListboxSetWidth` | Sets explicit listbox width |
| `ListboxSetCurrentByKey` | Selects item by data key |
| `ListboxSetData` | Updates item data (void*) |
| `ListboxInsertEntry` | Inserts item before another |
| `ListboxDeleteEntry` | Removes item by key |
| `ListboxGetEntry` | Reads back text + data for an item by index |
| `ListboxClearSelection` | Clears multi-select state |
| `TextboxSetHeight` | Changes textbox height post-creation |
| `TextboxSetColors` | Changes textbox colorset |
| `CheckboxTreeSetEntry` | Changes tree item text |
| `CheckboxTreeSetWidth` | Changes tree widget width |
| `Scale` (constructor) | Creates a progress bar widget |
| `ScaleSet` | Sets scale value |
| `ScaleSetColors` | Changes scale colors |

### Entry operations

| Subcommand | Notes |
|---|---|
| `EntrySetColors` | Changes entry colorset |
| `EntryGetCursorPosition` | Reads cursor position |
| `EntrySetCursorPosition` | Moves cursor within entry |
| `EntrySetFilter` | Registers bash function as keystroke filter |

### Form operations

| Subcommand | Notes |
|---|---|
| `FormSetSize` | Forces form to recalculate size |
| `FormGetCurrent` | Returns the currently focused component |
| `FormSetBackground` | Changes form background color |
| `FormSetCurrent` | Moves focus to a specific component |
| `FormSetHeight` | Constrains form height |
| `FormSetWidth` | Constrains form width |
| `FormAddHotKey` | Registers a key that exits the form |
| `FormGetScrollPosition` | Reads scroll position of scrollable form |
| `FormSetScrollPosition` | Sets scroll position of scrollable form |

### Component operations

| Subcommand | Notes |
|---|---|
| `ComponentDestroy` | Destroys a single component (not via FormDestroy) |
| `ComponentTakesFocus` | Controls whether a component is focusable |
| `ComponentAddDestroyCallback` | Registers bash expr to run on destroy |
| `GetScreenSize` | Reads terminal dimensions; easy to add |

### Grid operations

| Subcommand | Notes |
|---|---|
| `GridSimpleWindow` | Similar to GridBasicWindow; easy to add |
| `GridDestroy` | Alias for GridFree; just needs a dispatch-table test |

---

## Weak / shallow assertions in existing tests

These tests exist but only perform surface-level checks:

| Test | Weakness |
|---|---|
| `test_checkbox_set_flags_disable` | Only checks the label is visible, doesn't verify the checkbox is actually non-interactive |
| `test_listbox_setcurrent` | Doesn't verify which item is highlighted, only that "Cherry" is on screen (it would be anyway) |
| `test_checkboxtree_getentryvalue` | Asserts `val=[` appears but doesn't check the specific value `*` |
| `test_textbox_get_num_lines` | Asserts `n=[` exists but doesn't verify the exact count (should be 3) |
| `test_checkboxtree_setcurrent_getcurrent` | Only asserts result is non-empty, doesn't verify it matches the expected data key |

---

## Suggested test expansions (priority order)

### High priority — easy to add, cover important gaps

1. **`GetScreenSize`** — call after Init, verify cols/rows > 0
2. **`CenteredWindow`** — open a centered window, verify title appears
3. **`PopWindow`** — open two windows, pop one, verify only second is visible
4. **`PushHelpLine` / `PopHelpLine`** — set help text, verify it appears at bottom row
5. **`Scale` / `ScaleSet`** — create a progress bar and set a value
6. **`FormAddHotKey`** — register F12 as hotkey, press it, verify FormRun returns HOTKEY
7. **`EntrySetFilter`** — register a filter that blocks digits, verify digits are not entered
8. **`ComponentAddDestroyCallback`** — register a callback, destroy component, verify callback fires
9. **`ComponentTakesFocus`** — create two buttons, disable focus on one, verify Tab skips it
10. **`ComponentDestroy`** — create and destroy a standalone component

### Medium priority — useful but less critical

11. **`ListboxGetEntry`** — add entry, read it back with GetEntry, verify text and data
12. **`ListboxDeleteEntry`** — add entries, delete one, verify count decreases
13. **`ListboxInsertEntry`** — insert before existing entry, verify order
14. **`ListboxSetCurrentByKey`** — set by data key, verify correct item focused
15. **`FormGetCurrent` / `FormSetCurrent`** — set focus, verify it moved
16. **`FormAddHotKey` + `FormRun`** — verify HOTKEY exit with key value
17. **`GridSimpleWindow`** — like GridBasicWindow test
18. **`SetColor`** — set one colorset, render text, verify no crash
19. **`ReflowText`** — strengthen assertion: verify exact width/height values

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

### Extra arguments are silently ignored

Wrappers do not reject extra trailing arguments.  This is consistent
with how most bash builtins behave, but it means typos in argument
lists may go unnoticed.  Consider adding a warning when extra args are
detected (especially for `call_newt`-based wrappers where the expected
count is known at compile time).
