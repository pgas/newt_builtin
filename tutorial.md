# Writing Shell Scripts Using the `newt` Bash Builtin

This tutorial is a bash translation of Erik Troan's original C tutorial for
[libnewt](https://pagure.io/newt).  Every C example has been rewritten to use
the `newt` bash loadable builtin from this repository.  The complete, runnable
scripts are in the [`examples/`](examples/) directory.

---

## 1  Introduction

### 1.1  Background

`newt` is a terminal-based window and widget library originally designed for
Red Hat Linux's installer.  Its design priorities:

- **Not event-driven** — your script calls a function to collect input and
  then continues; there are no callbacks called by the library.
- **Modal windows** — windows must be created and destroyed as a stack; only
  the top window can be active.
- **Keyboard-only input.**
- **Small footprint** — the library is layered on S-Lang for low-level screen
  management.

### 1.2  Designing `newt` applications

Because `newt` is not event-driven and forces modal windows, `newt` scripts
look very much like ordinary sequential programs.  Converting a command-line
tool that uses simple prompts is often straightforward.

### 1.3  Components

Displayable items are called **components**, analogous to widgets in other
toolkits.  There are two kinds:

- **Forms** — logical groupings of other components.  When an application
  wants input, it "runs a form", which hands control to the form until the
  user is done.
- **Everything else** — buttons, labels, entry boxes, checkboxes, listboxes,
  textboxes, etc.

In the bash builtin every component is represented as a shell variable holding
an opaque pointer string (e.g. `0x5b10045614f0`).

### 1.4  Conventions

The builtin mirrors the C naming conventions:

| Convention | Example |
|---|---|
| Subcommand = C function name minus the `newt` prefix | `newtOpenWindow` → `newt OpenWindow` |
| First two args are always `left top` | `newt Label 5 3 "text"` |
| Width before height | `newt OpenWindow 10 5 40 8 "Title"` |
| Store return value with `-v varname` | `newt -v lbl Label 5 3 "text"` |

---

## 2  Basic `newt` Functions

### 2.1  Starting and Ending `newt` Services

Every script must call `newt Init` first and `newt Finished` when done.

| C function | Bash builtin |
|---|---|
| `newtInit()` | `newt Init` |
| `newtCls()` | `newt Cls` |
| `newtFinished()` | `newt Finished` |

`newt Init` places the terminal in raw mode.  Call `newt Cls` immediately
afterwards to clear the screen.  Register `newt Finished` in a `trap` so the
terminal is always restored even if the script exits unexpectedly:

```bash
newt Init
trap 'newt Finished' EXIT
newt Cls
```

### 2.2  Handling Keyboard Input

| C function | Bash builtin |
|---|---|
| `newtWaitForKey()` | `newt WaitForKey` |
| `newtClearKeyBuffer()` | `newt ClearKeyBuffer` |

`newt WaitForKey` blocks until a key is pressed, then discards the keystroke.
`newt ClearKeyBuffer` discards anything already in the input buffer.

### 2.3  Drawing on the Root Window

The **root window** is the background behind all windows.  Two mechanisms
exist for writing text there.

**`DrawRootText`** places arbitrary text at a position.  Negative coordinates
are measured from the opposite edge (`-1` = last column/row):

```bash
newt DrawRootText 0 0 "Some root text"
newt DrawRootText -25 -2 "Root text in the other corner"
```

**The help line** is the last row of the terminal and is managed as a stack
that mirrors the window stack:

| C function | Bash builtin |
|---|---|
| `newtPushHelpLine(text)` | `newt PushHelpLine "text"` |
| `newtPopHelpLine()` | `newt PopHelpLine` |

Pass `""` (empty string) to clear the help line; omit the argument (or omit
`""`) to show `newt`'s default help line.

### 2.4  Suspending `newt` Applications

By default the suspend key (`^Z`) is ignored.  To restore normal suspend
behaviour register a bash function or expression as the callback:

```bash
function do_suspend {
    newt Suspend
    kill -TSTP $$          # send SIGTSTP to ourselves
    newt Resume
}
newt SetSuspendCallback do_suspend
```

| C function | Bash builtin |
|---|---|
| `newtSetSuspendCallback(cb)` | `newt SetSuspendCallback bashFn` |
| `newtSuspend()` | `newt Suspend` |
| `newtResume()` | `newt Resume` |

### 2.5  Refreshing the Screen

S-Lang only flushes display updates right before waiting for user input.
Force an immediate update with:

```bash
newt Refresh
```

Useful when displaying progress without waiting for user input.

### 2.6  Other Miscellaneous Functions

| C function | Bash builtin |
|---|---|
| `newtBell()` | `newt Bell` |
| `newtGetScreenSize(&cols,&rows)` | `newt GetScreenSize COLS ROWS` |

`GetScreenSize` writes directly into two named variables instead of using
`-v` (it has two output values):

```bash
newt GetScreenSize COLS ROWS
echo "Terminal is ${COLS}×${ROWS}"
```

### 2.7  Basic `newt` Example

> **Script:** [`examples/tutorial_2_6.sh`](examples/tutorial_2_6.sh)

```bash
newt Init
trap 'newt Finished' EXIT
newt Cls

newt DrawRootText 0 0 "Some root text"
help="Root text in the other corner"
newt DrawRootText -${#help} -2 "$help"

newt PushHelpLine ""        # clear help line
newt Refresh
sleep 1

newt PushHelpLine "A help line"
newt Refresh
sleep 1

newt PopHelpLine
newt Refresh
sleep 1
```

---

## 3  Windows

Windows must be destroyed in the reverse of the order they were created; only
the top window may be active at any time.

### 3.1  Creating Windows

| C function | Bash builtin |
|---|---|
| `newtCenteredWindow(w,h,title)` | `newt CenteredWindow w h "title"` |
| `newtOpenWindow(l,t,w,h,title)` | `newt OpenWindow l t w h "title"` |

Pass `""` or omit `title` for a window without a title.

### 3.2  Destroying Windows

```bash
newt PopWindow   # removes the top window
```

---

## 4  Components

### 4.1  Introduction to Forms

A form groups components together and manages user interaction.  The typical
lifecycle is:

```bash
newt -v form Form                          # create the form
newt FormAddComponent  "$form" "$comp"     # add one component
newt FormAddComponents "$form" "$a" "$b"   # add several at once (varargs)
newt RunForm "$form"                       # run (blocks until user finishes)
newt FormDestroy "$form"                   # free the form and its components
```

| C function | Bash builtin |
|---|---|
| `newtForm(NULL,NULL,0)` | `newt -v f Form` |
| `newtFormAddComponent(f,co)` | `newt FormAddComponent "$f" "$co"` |
| `newtFormAddComponents(f,…,NULL)` | `newt FormAddComponents "$f" "$a" "$b" …` |
| `newtRunForm(f)` | `newt RunForm "$f"` |
| `newtFormDestroy(f)` | `newt FormDestroy "$f"` |

`Form` accepts optional arguments: `newt -v f Form [vertBar [help [flags]]]`.
For the common case all three can be omitted.

### 4.2  General Component Manipulation

```bash
newt ComponentAddCallback "$co" 'bash_expression_or_function_name'
newt ComponentTakesFocus  "$co" 1   # 1 = takes focus; 0 = skip during traversal
```

### 4.3  Buttons

| C function | Bash builtin |
|---|---|
| `newtButton(l,t,text)` | `newt -v b Button l t "text"` |
| `newtCompactButton(l,t,text)` | `newt -v b CompactButton l t "text"` |

Full buttons look better but occupy more space.  Compact buttons are a single
row.

### 4.4  Button Example

> **Script:** [`examples/tutorial_4_4.sh`](examples/tutorial_4_4.sh)

```bash
newt Init
trap 'newt Finished' EXIT
newt Cls
newt OpenWindow 10 5 40 6 "Button Sample"

newt -v b1 Button        10 1 "Ok"
newt -v b2 CompactButton 22 2 "Cancel"

newt -v form Form
newt FormAddComponents "$form" "$b1" "$b2"
newt RunForm "$form"
newt FormDestroy "$form"
```

### 4.5  Labels

Labels display text and accept no input.

| C function | Bash builtin |
|---|---|
| `newtLabel(l,t,text)` | `newt -v lbl Label l t "text"` |
| `newtLabelSetText(co,text)` | `newt LabelSetText "$lbl" "new text"` |

### 4.6  Entry Boxes

Entry boxes let the user type a string.

| C function | Bash builtin |
|---|---|
| `newtEntry(l,t,init,w,&ptr,flags)` | `newt -v e Entry l t "init" w [flags]` |
| `newtEntrySet(co,val,cursor)` | `newt EntrySet "$e" "val" 0` |
| `newtEntryGetValue(co)` | `newt -v val EntryGetValue "$e"` |
| `newtEntrySetFilter(co,fn,data)` | `newt EntrySetFilter "$e" bash_fn` |

Common flags (use `${NEWT_FLAG[NAME]}`):

| Flag | Meaning |
|---|---|
| `SCROLL` | Allow entry text wider than the box |
| `HIDDEN` | Hide text (password input) |
| `RETURNEXIT` | Return key exits the form |

### 4.7  Label & Entry Example

> **Script:** [`examples/tutorial_4_6.sh`](examples/tutorial_4_6.sh)

```bash
newt Init
trap 'newt Finished' EXIT
newt Cls
newt OpenWindow 10 5 40 8 "Entry and Label Sample"

newt -v label  Label 1 1 "Enter a string"
newt -v entry  Entry 16 1 "sample" 20 \
    $(( ${NEWT_FLAG[SCROLL]} | ${NEWT_FLAG[RETURNEXIT]} ))
newt -v button Button 17 3 "Ok"

newt -v form Form
newt FormAddComponents "$form" "$label" "$entry" "$button"
newt RunForm "$form"

newt -v message EntryGetValue "$entry"
newt FormDestroy "$form"
# newt Finished is called by the EXIT trap

echo "Final string was: $message"
```

### 4.8  Checkboxes

Checkboxes cycle through an arbitrary sequence of characters.  The default
sequence is `" *"` (space = unchecked, `*` = checked).

| C function | Bash builtin |
|---|---|
| `newtCheckbox(l,t,text,def,seq,&res)` | `newt -v cb Checkbox l t "text" [def [seq]]` |
| `newtCheckboxGetValue(co)` | `newt -v val CheckboxGetValue "$cb"` |

### 4.9  Radio Buttons

Radio buttons are grouped through the `prevButton` parameter.  Pass `""`
(empty string) for the first button in a new group.

| C function | Bash builtin |
|---|---|
| `newtRadiobutton(l,t,text,isDefault,prev)` | `newt -v rb Radiobutton l t "text" isDefault "$prev"` |
| `newtRadioGetCurrent(setMember)` | `newt -v cur RadioGetCurrent "$rb0"` |

### 4.10  Checkbox & Radio Button Example

> **Script:** [`examples/tutorial_4_8.sh`](examples/tutorial_4_8.sh)

```bash
newt Init
trap 'newt Finished' EXIT
newt Cls
newt OpenWindow 10 5 40 11 "Checkboxes and Radio buttons"

# Checkbox with a custom 3-state sequence: ' ', '*', 'X'
newt -v checkbox Checkbox 1 1 "A checkbox" ' ' " *X"

# Radio group: pass "" as prevButton for the first button
newt -v rb0 Radiobutton 1 3 "Choice 1" 1 ""
newt -v rb1 Radiobutton 1 4 "Choice 2" 0 "$rb0"
newt -v rb2 Radiobutton 1 5 "Choice 3" 0 "$rb1"

newt -v button Button 1 7 "Ok"

newt -v form Form
newt FormAddComponents "$form" "$checkbox" "$rb0" "$rb1" "$rb2" "$button"
newt RunForm "$form"

# Read results before destroying the form
newt -v cbValue  CheckboxGetValue "$checkbox"
newt -v current  RadioGetCurrent  "$rb0"
newt FormDestroy "$form"
# newt Finished is called by the EXIT trap

for i in 0 1 2; do
    rb_var="rb${i}"
    if [[ "$current" == "${!rb_var}" ]]; then
        echo "radio button picked: $i"
        break
    fi
done
echo "checkbox value: '$cbValue'"
```

### 4.11  Scales

Scales display a horizontal progress bar.

| C function | Bash builtin |
|---|---|
| `newtScale(l,t,width,fullValue)` | `newt -v sc Scale l t width fullValue` |
| `newtScaleSet(co,amount)` | `newt ScaleSet "$sc" amount` |

### 4.12  Textboxes

Textboxes display a block of (optionally scrollable) text.

| C function | Bash builtin |
|---|---|
| `newtTextbox(l,t,w,h,flags)` | `newt -v tb Textbox l t w h flags` |
| `newtTextboxSetText(co,text)` | `newt TextboxSetText "$tb" "text"` |
| `newtTextboxReflowed(l,t,text,w,fd,fu,flags)` | `newt -v tb TextboxReflowed l t "text" w fd fu flags` |
| `newtTextboxGetNumLines(co)` | `newt -v n TextboxGetNumLines "$tb"` |

Textbox flags:

| Flag | Meaning |
|---|---|
| `${NEWT_FLAG[WRAP]}` | Wrap text to fit the box width |
| `${NEWT_FLAG[SCROLL]}` | Make the textbox scrollable (adds 2 to its width) |

`TextboxReflowed` creates a textbox, reflows `text` to a target width
(within `±flexDown/flexUp`), and fills the box — all in one call.

### 4.13  Textbox Example

> **Script:** [`examples/tutorial_4_10.sh`](examples/tutorial_4_10.sh)

```bash
newt Init
trap 'newt Finished' EXIT
newt Cls

message=$'This is a pretty long message. It will be displayed \
in a newt textbox, and illustrates how to construct \
a textbox from arbitrary text which may not have \
very good line breaks.\n\nNotice how literal newline characters are \
respected, and may be used to force line breaks and blank lines.'

# Create the reflowed textbox first — we need its height to size the window.
newt -v text TextboxReflowed 1 1 "$message" 30 5 5 0
newt -v numlines TextboxGetNumLines "$text"

newt -v button Button 12 $(( numlines + 2 )) "Ok"

# Open the window sized to fit the textbox.
newt OpenWindow 10 5 37 $(( numlines + 7 )) "Textboxes"

newt -v form Form
newt FormAddComponents "$form" "$text" "$button"
newt RunForm "$form"
newt FormDestroy "$form"
```

### 4.14  Scrollbars

Vertical scrollbars can be attached to forms or listboxes.

| C function | Bash builtin |
|---|---|
| `newtVerticalScrollbar(l,t,h,norm,thumb)` | `newt -v sb VerticalScrollbar l t h normColorset thumbColorset` |

Colorset constants are available as `${NEWT_COLORSET[NAME]}`, e.g.
`${NEWT_COLORSET[WINDOW]}` and `${NEWT_COLORSET[ACTCHECKBOX]}`.

### 4.15  Listboxes

Each listbox entry is an (text, key) pair.  The key is an opaque value
returned by `ListboxGetCurrent`.

| C function | Bash builtin |
|---|---|
| `newtListbox(l,t,height,flags)` | `newt -v lb Listbox l t height flags` |
| `newtListboxAppendEntry(co,text,data)` | `newt ListboxAppendEntry "$lb" "text" data` |
| `newtListboxGetCurrent(co)` | `newt -v key ListboxGetCurrent "$lb"` |
| `newtListboxSetCurrent(co,num)` | `newt ListboxSetCurrent "$lb" num` |
| `newtListboxSetCurrentByKey(co,key)` | `newt ListboxSetCurrentByKey "$lb" key` |
| `newtListboxSetWidth(co,width)` | `newt ListboxSetWidth "$lb" width` |
| `newtListboxSetEntry(co,key,text)` | `newt ListboxSetEntry "$lb" key "text"` |
| `newtListboxInsertEntry(co,text,data,key)` | `newt ListboxInsertEntry "$lb" "text" data key` |
| `newtListboxDeleteEntry(co,key)` | `newt ListboxDeleteEntry "$lb" key` |
| `newtListboxClear(co)` | `newt ListboxClear "$lb"` |

`ListboxAddEntry` is an alias for `ListboxAppendEntry`.

Listbox flags (combine with `$(( ... | ... ))`):

| Flag | Meaning |
|---|---|
| `${NEWT_FLAG[SCROLL]}` | Scrollable list |
| `${NEWT_FLAG[RETURNEXIT]}` | Return on item exits form |
| `${NEWT_FLAG[BORDER]}` | Draw a frame around the listbox |
| `${NEWT_FLAG[MULTIPLE]}` | Allow multiple selections |

For multiple-selection listboxes:

| C function | Bash builtin |
|---|---|
| `newtListboxClearSelection(co)` | `newt ListboxClearSelection "$lb"` |
| `newtListboxSelectItem(co,key,sense)` | `newt ListboxSelectItem "$lb" key sense` |

`sense` is one of `${NEWT_FLAGS_SENSE[SET]}`, `${NEWT_FLAGS_SENSE[RESET]}`, or
`${NEWT_FLAGS_SENSE[TOGGLE]}`.

### 4.16  Advanced Forms

By default `newt` exits a form when **F12** is pressed.  Treat F12 as an
implicit "Ok".

```bash
# Add a hot key that exits the form when pressed
newt FormAddHotKey "$form" "${NEWT_KEY[F1]}"

# Watch a file descriptor — e.g. a coproc pipe
newt FormWatchFd "$form" "$fd" "${NEWT_FD[READ]}"
```

Use `newt FormRun "$form" REASON VALUE` instead of `newt RunForm` for the
structured exit information:

```bash
newt FormRun "$form" REASON VALUE
case "$REASON" in
    "${NEWT_EXIT[COMPONENT]}") echo "button pressed: $VALUE" ;;
    "${NEWT_EXIT[HOTKEY]}")    echo "hot key: $VALUE" ;;
    "${NEWT_EXIT[FDREADY]}")   echo "fd ready: $VALUE" ;;
esac
```

Other form helpers:

| C function | Bash builtin |
|---|---|
| `newtFormSetHeight(f,h)` | `newt FormSetHeight "$form" h` |
| `newtFormSetWidth(f,w)` | `newt FormSetWidth "$form" w` |
| `newtFormSetBackground(f,color)` | `newt FormSetBackground "$form" color` |
| `newtFormGetCurrent(f)` | `newt -v co FormGetCurrent "$form"` |
| `newtFormSetCurrent(f,co)` | `newt FormSetCurrent "$form" "$co"` |
| `newtDrawForm(f)` | `newt DrawForm "$form"` |

---

## 5  Grids

Grids lay out components without hard-coding pixel positions.

```bash
newt -v g GridHStacked "$co1" padLeft padTop padRight padBottom flags \
                        "$co2" padLeft padTop padRight padBottom flags \
                        "${NEWT_ARG[LAST]}"
newt GridWrappedWindow "$g" "Window Title"
newt -v form Form
newt FormAddComponents "$form" ...
newt RunForm "$form"
newt FormDestroy "$form"
newt PopWindow
newt GridFree "$g" 1
```

See [`examples/tutorial_4_4.sh`](examples/tutorial_4_4.sh) and the
[`functional_test/test_grid.py`](functional_test/test_grid.py) for complete
grid examples.

---

## 6  Loading the Builtin

```bash
# Load once per shell session
enable -f build/src/newt.so newt

# All example scripts contain this portable loader:
for lib in libs/ ../libs/ build/src/ ../build/src/; do
    if [[ -f $lib/newt.so ]]; then
        enable -f "$lib/newt.so" newt
        break
    fi
done
if ! type newt &>/dev/null; then
    echo >&2 "unable to load the newt builtin"
    exit 1
fi
```

Build the shared library:

```bash
cmake -B build && cmake --build build
```

---

## 7  Quick Reference

### Constants

| Shell variable | Contents |
|---|---|
| `${NEWT_FLAG[NAME]}` | `NEWT_FLAG_*` integer bitmasks |
| `${NEWT_COLORSET[NAME]}` | `NEWT_COLORSET_*` integer values |
| `${NEWT_KEY[NAME]}` | `NEWT_KEY_*` key codes |
| `${NEWT_EXIT[COMPONENT\|HOTKEY\|FDREADY\|…]}` | `newtExitReason` enum values |
| `${NEWT_FD[READ\|WRITE\|EXCEPT]}` | `NEWT_FD_*` flags for `FormWatchFd` |
| `${NEWT_FLAGS_SENSE[SET\|RESET\|TOGGLE]}` | `newtFlagsSense` values |
| `${NEWT_ARG[LAST\|APPEND]}` | Sentinel integers for grid/listbox calls |

### `newt -v` vs positional output

Most subcommands return a single value captured with `-v varname`.
Exceptions that write to named variables directly:

| Subcommand | Output variables |
|---|---|
| `newt GetScreenSize COLS ROWS` | `COLS`, `ROWS` |
| `newt ListboxGetEntry lb keyVar textVar dataVar` | `textVar`, `dataVar` |
| `newt FormRun form REASON VALUE` | `REASON`, `VALUE` |
