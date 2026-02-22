# GitHub Copilot Instructions — newt_builtin2

## Project overview

`newt_builtin2` is a **C++17 bash loadable builtin** that wraps [libnewt](https://pagure.io/newt) so TUI dialogs can be driven directly from shell scripts without shelling out to a helper.

```bash
enable -f build/src/newt.so newt   # load once per shell session
newt Init
newt -v lbl Label 10 2 "Hello"
newt -v f   Form NULL "" 0
newt FormAddComponent "$f" "$lbl"
newt RunForm "$f"
newt Finished
```

The single `newt` builtin dispatches to ~95 sub-commands named after the
corresponding `newtXxx` C function with the leading `newt` prefix stripped and
PascalCase preserved:  `newtOpenWindow` → `OpenWindow`.

---

## Repository layout

```
src/
  newt_arg_parser.hpp   # template engine: from_string, to_bash_string, call_newt
  newt_wrappers.cpp     # all wrap_* functions + dispatch table
  newt_wrappers.hpp
  newt.cpp              # bash builtin entry-point (newt_builtin)
test/
  stubs/
    bash_stubs.hpp      # WORD_LIST, bind_variable, legal_number, EXECUTION_*
    newt_stubs.hpp      # newtComponent, newtGrid, newtCallback, enums
    word_list_builder.hpp
  test_from_string.cpp
  test_to_bash_string.cpp
  test_parse_args.cpp
  test_call_newt.cpp
  test_components.cpp   # component functions (newt.h lines 141-164)
  test_wrappers.cpp     # all remaining wrappers
functional_test/
  conftest.py           # pexpect + pyte fixtures
  test_*.py             # one file per widget family
examples/               # runnable bash scripts
```

---

## Core template: `call_newt`

Most wrappers are a single line that delegates everything to the template:

```cpp
// In newt_wrappers.cpp:
static int wrap_OpenWindow(char* v, WORD_LIST* a) {
    return call_newt("OpenWindow", "left top width height title",
                     newtOpenWindow, v, a);
}
```

`call_newt` deduces all argument and return types from the function pointer,
calls `parse_args` to walk `WORD_LIST*` using the matching `from_string`
overload for each parameter, then calls the real function via `std::apply`.
- **void return**: binds nothing, returns `EXECUTION_SUCCESS`.
- **non-void return**: converts result with `to_bash_string` and binds it to
  the variable named by `v` (from `newt -v varname …`).
- **too few arguments**: prints a usage line and returns `EXECUTION_FAILURE`.

The second string argument is purely documentation (printed on error); it does
not affect parsing.

---

## Adding a new wrapper

### 1. Simple case — direct `call_newt` delegation

Add a one-liner in the "thin wrappers" section of `newt_wrappers.cpp` and an
entry in the dispatch table at the bottom:

```cpp
// wrapper
static int wrap_TextboxSetText(char* v, WORD_LIST* a) {
    return call_newt("TextboxSetText", "co text", newtTextboxSetText, v, a);
}

// dispatch table entry
{ "TextboxSetText", wrap_TextboxSetText },
```

### 2. Special case — hand-written wrapper

Use this pattern when `call_newt` cannot model the function (e.g. output
pointers, variadic args, optional args, side-channel state):

```cpp
static int wrap_GetScreenSize(char* /*v*/, WORD_LIST* a) {
    const char* cols_var;
    const char* rows_var;

    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, cols_var)) goto usage;
    if (!a->next) goto usage; a = a->next;
    if (!from_string(a->word->word, rows_var)) goto usage;
    {
        int cols = 0, rows = 0;
        newtGetScreenSize(&cols, &rows);
        bind_variable(const_cast<char*>(cols_var),
                      const_cast<char*>(to_bash_string(cols).c_str()), 0);
        bind_variable(const_cast<char*>(rows_var),
                      const_cast<char*>(to_bash_string(rows).c_str()), 0);
    }
    return EXECUTION_SUCCESS;
usage:
    std::fprintf(stderr, "newt: usage: newt GetScreenSize colsVar rowsVar\n");
    return EXECUTION_FAILURE;
}
```

Pattern rules:
- Walk `WORD_LIST*` by advancing `a = a->next` before each argument.
- Use `from_string(a->word->word, typed_var)` for parsing.
- Use `to_bash_string(value)` + `bind_variable(...)` to expose results.
- Always `goto usage` on parse failure; print to `stderr` and return
  `EXECUTION_FAILURE`.

### 3. Aliases

When libnewt defines a macro alias (e.g. `#define newtListboxAddEntry
newtListboxAppendEntry`), expose both names in the dispatch table pointing to
the same wrapper:

```cpp
{ "ListboxAppendEntry", wrap_ListboxAppendEntry },
{ "ListboxAddEntry",    wrap_ListboxAppendEntry },   // macro alias
```

---

## `from_string` / `to_bash_string` extension points

Add overloads to `src/newt_arg_parser.hpp` when a new C type needs parsing:

| C type | Notes |
|--------|-------|
| `int`, `unsigned int`, `long long`, `unsigned long long` | via `legal_number` |
| `char*` / `const char*` | direct alias of the bash string |
| `char` | first character of the string |
| `newtComponent`, `newtGrid` | `%p` hex or `"NULL"` |
| `void*` / `const void*` | decimal integer first (listbox keys), then `%p`, then `"NULL"` |
| `newtCallback`, `newtSuspendCallback`, `newtEntryFilter` | `%p` hex |
| `newtFlagsSense` | integer cast from `legal_number` |

> **Important**: `void*` parsing tries `legal_number` (decimal) **before**
> `sscanf("%p")`.  On Linux/glibc, `sscanf("42", "%p")` yields `0x42` = 66,
> not 42.  Always keep decimal-first ordering.

---

## Unit tests (Catch2)

Tests live in `test/` and compile against stubs — no display or real bash
installation needed.

### Pattern for a new `call_newt`-based wrapper

```cpp
// Fake with matching signature
static int fake_my_fn(newtComponent, int) { return 0; }

TEST_CASE("MyCommand: co num → SUCCESS", "[wrappers][MyCommand]") {
    WordListBuilder wl{"cmd", "NULL", "5"};
    CHECK(call_newt("MyCommand", "co num", fake_my_fn, nullptr, wl.head())
          == EXECUTION_SUCCESS);
}
TEST_CASE("MyCommand: too few args → FAILURE", "[wrappers][MyCommand]") {
    WordListBuilder wl{"cmd", "NULL"};   // missing num
    CHECK(call_newt("MyCommand", "co num", fake_my_fn, nullptr, wl.head())
          == EXECUTION_FAILURE);
}
```

Key helpers:
- `WordListBuilder{"cmd", "arg1", "arg2", ...}` — builds a `WORD_LIST*`
  (first word is the command name, skipped by the wrappers).
- `clear_bound_vars()` — resets the stub binding store before testing a
  non-void return.
- `last_bound("varname")` — returns `const std::string*` of the last value
  bound to that variable name.
- `CO = "NULL"` — sentinel for a null `newtComponent`.

### Running unit tests

```bash
cmake -B build && cmake --build build
ctest --test-dir build --output-on-failure
```

---

## Functional tests (pexpect + pyte)

Tests in `functional_test/` spawn real bash in a pty, send commands, render
the terminal screen with pyte, and assert on visible text.

### Pattern for a new widget test

```python
def test_mything_visible(bash_newt):
    bash_newt.sendline(
        b"newt Init && newt Cls && "
        b'newt OpenWindow 5 3 50 10 "My Test" && '
        b'newt -v w MyWidget 3 2 "Label" && '
        b'newt -v f Form NULL "" 0 && '
        b'newt FormAddComponents "$f" "$w" && '
        b'newt RunForm "$f" && '
        b'newt FormDestroy "$f" && '
        b"newt Finished"
    )
    screen = render(bash_newt, initial_timeout=2.0)
    rows = screen_rows(screen)
    full = screen_text(screen)

    assert any("Label" in r for r in rows), \
        f"Widget label not visible.\n{full}"

    bash_newt.send(b"\n")   # unblock RunForm
```

For tests that read back a value after the form exits:

```python
bash_newt.send(b"\n")          # unblock RunForm
time.sleep(0.5)
screen = render(bash_newt, initial_timeout=1.5, drain_timeout=0.3)
rows = screen_rows(screen)
assert any("val=[expected]" in r for r in rows), ...
```

`conftest.py` provides:
- `bash_newt` fixture — pexpect spawn with `enable -f build/src/newt.so newt`
- `render(child, initial_timeout, drain_timeout)` — feeds pty output into a
  pyte screen and returns it
- `screen_rows(screen)` — list of stripped non-empty rows
- `screen_text(screen)` — full screen dump for assertion messages

### Running functional tests

```bash
cmake --build build          # ensure newt.so is current
cd functional_test
uv sync                      # one-time: installs pexpect, pyte, pytest
uv run pytest -v
```

---

## Known special-case wrappers

These functions cannot use `call_newt` and are fully hand-written. When
modifying them, be careful about argument walking and output binding.

| Subcommand | Reason |
|---|---|
| `GetScreenSize` | Two output-pointer arguments |
| `SetColors` | Takes a `struct newtColors` by value |
| `SetSuspendCallback` | Registers a C shim; stores bash function name in `g_suspend_callback_fn` |
| `ComponentAddDestroyCallback` | Registers a C shim; stores bash expression in `g_destroy_callbacks` |
| `ListboxGetEntry` | Two output pointers (text + data) |
| `EntrySetFilter` | Registers a C shim; stores bash function name in `g_entry_filters` |
| `FormAddComponents` | Variadic: walks the remaining `WORD_LIST*` args |
| `Entry` (constructor) | Optional `flags` argument |
| `Form` (constructor) | All three args optional |
| `Checkbox` (constructor) | Optional `defValue` and `seq` arguments |

---

## Naming conventions

- Subcommand names are **PascalCase** matching `newtXxx` with the `newt`
  prefix stripped.  Do not lowercase or abbreviate: `OpenWindow` not
  `openwindow` or `open_window`.
- Wrapper functions are named `wrap_SubcommandName`.
- Fake functions in unit tests are named `fake_<description>`.
- Global side-channel maps are named `g_<purpose>`.

---

## Checklist when adding a new wrapper

1. Look up the C signature in `/usr/include/newt.h`.
2. Check whether `call_newt` can model it (no output pointers, no variadic,
   no optional args, no side-channel state).
3. Add `wrap_Xxx` to `newt_wrappers.cpp` and a `{ "Xxx", wrap_Xxx }` entry in
   the dispatch table.
4. If the function is a macro alias of another, add both names to the dispatch
   table.
5. Add unit tests to `test/test_wrappers.cpp` (or `test_components.cpp` for
   component constructors): one SUCCESS case and one FAILURE (too few args)
   case per wrapper.
6. If the wrapper creates a visible widget, add a functional test in
   `functional_test/test_<family>.py`.
7. Run `ctest --test-dir build` and confirm all tests pass before committing.
