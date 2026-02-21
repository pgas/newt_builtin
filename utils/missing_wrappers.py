#!/usr/bin/env python3
"""
Report newt API functions that have no wrapper in newt_wrappers.cpp.

Usage:
    python3 missing_wrappers.py [--header /usr/include/newt.h]
                                [--wrappers src/newt_wrappers.cpp]
"""

import argparse
import re
import sys
from pathlib import Path


def parse_header(path: Path) -> set[str]:
    """Return the set of bare names (without the 'newt' prefix) of every
    function declared in newt.h."""
    text = path.read_text()
    # Match identifiers that look like function calls: newtXxx(
    return {
        m.group(1)
        for m in re.finditer(r'\bnewt([A-Z]\w+)\s*\(', text)
    }


def parse_wrappers(path: Path) -> set[str]:
    """Return the set of bare names registered in the dispatch table.

    Looks for lines of the form:
        { "SomeName",   wrap_SomeName  },
    """
    text = path.read_text()
    return {
        m.group(1)
        for m in re.finditer(r'\{\s*"([A-Z]\w+)"\s*,\s*wrap_\w+', text)
    }


def group_by_prefix(names: set[str]) -> dict[str, list[str]]:
    """Group bare names by their camel-case prefix (first capital run)."""
    groups: dict[str, list[str]] = {}
    for name in sorted(names):
        # split at first transition from upper+lower to next uppercase run
        m = re.match(r'([A-Z][a-z]+(?:[A-Z][a-z]+)?)', name)
        prefix = m.group(1) if m else name
        groups.setdefault(prefix, []).append(name)
    return groups


def main() -> None:
    repo_root = Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser(description="Find unwrapped newt API functions.")
    parser.add_argument(
        "--header",
        type=Path,
        default=Path("/usr/include/newt.h"),
        help="Path to newt.h (default: /usr/include/newt.h)",
    )
    parser.add_argument(
        "--wrappers",
        type=Path,
        default=repo_root / "src" / "newt_wrappers.cpp",
        help="Path to newt_wrappers.cpp",
    )
    args = parser.parse_args()

    if not args.header.exists():
        sys.exit(f"Header not found: {args.header}")
    if not args.wrappers.exists():
        sys.exit(f"Wrappers file not found: {args.wrappers}")

    header_fns  = parse_header(args.header)
    wrapped_fns = parse_wrappers(args.wrappers)

    missing = header_fns - wrapped_fns
    extra   = wrapped_fns - header_fns   # sanity check

    print(f"newt.h exports   : {len(header_fns):3d} functions")
    print(f"Wrapped           : {len(wrapped_fns):3d} functions")
    print(f"Missing wrappers  : {len(missing):3d} functions")
    if extra:
        print(f"Wrapped but not in header (renamed/removed?): {len(extra)}")
    print()

    groups = group_by_prefix(missing)
    for prefix, names in sorted(groups.items()):
        print(f"  {prefix}")
        for name in names:
            print(f"    newt{name}")

    if extra:
        print("\nIn wrappers but NOT in header:")
        for name in sorted(extra):
            print(f"  newt{name}")


if __name__ == "__main__":
    main()
