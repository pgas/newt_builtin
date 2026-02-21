# Functional Tests

Functional tests for the `newt` bash builtin.  Each test spawns a real `bash`
process inside a pseudo-terminal (pty) via **pexpect**, sends commands that
drive the `newt` TUI, and then asserts on the *rendered* screen content using
**pyte** (a pure-Python VT100 emulator) — no raw escape-sequence parsing
required.

## Prerequisites

| Requirement | Notes |
|---|---|
| Python ≥ 3.11 | |
| [uv](https://docs.astral.sh/uv/getting-started/installation/) | `curl -LsSf https://astral.sh/uv/install.sh \| sh` |
| Built `newt.so` | See the top-level README – run `cmake --build build` first |

## Setup

```bash
# From the repo root
cd functional_test

# Create a virtual environment and install dependencies (one-time)
uv sync
```

`uv sync` reads `pyproject.toml`, creates `.venv/` inside `functional_test/`,
and installs `pytest`, `pexpect`, and `pyte`.

## Running the tests

```bash
# From functional_test/
uv run pytest -v

# Run a single file
uv run pytest -v test_drawroottext.py

# Run a single test
uv run pytest -v test_label.py::test_label_text_visible
```

## Test overview

### `test_drawroottext.py`

| Test | What it checks |
|---|---|
| `test_drawroottext_top_left` | `newt DrawRootText 0 0 "…"` appears on row 0 |
| `test_drawroottext_offset` | `newt DrawRootText 5 2 "…"` appears at row 2, col 5 |

### `test_label.py`

| Test | What it checks |
|---|---|
| `test_label_text_visible` | Window title and label text both appear after `RunForm` |
| `test_label_set_text` | `newt LabelSetText` updates the visible label text |

## How it works

```
pexpect.spawn('bash')
    │  sendline(newt commands)
    │
    ▼
pty (pseudo-terminal)
    │  raw bytes (VT100 escape sequences + text)
    │
    ▼
pyte.ByteStream → pyte.Screen
    │  renders escape codes into a 2-D character grid
    │
    ▼
test assertions on screen.buffer[row][col]
```

`conftest.py` provides:
- `bash_newt` fixture — isolated bash + newt session per test
- `drain()` — reads all pending pty output with a configurable timeout
- `render()` — drains and returns a `pyte.Screen`
- `screen_rows()` / `screen_text()` — humanise the screen for assertions and failure messages
