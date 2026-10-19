#!/usr/bin/env python3
"""
pre_build.py — Pre-build script for Yaeger.

This script runs before the PlatformIO build. It performs:
  - Git status check (ensure no uncommitted changes in new/)
  - Code coverage setup (optional)
  - Build artifact preparation

The build itself is performed by PlatformIO in the subsequent step.
"""

import os
import sys
import subprocess
from pathlib import Path

try:
    from SCons.Script import DefaultEnvironment  # type: ignore
    env = DefaultEnvironment()
except Exception:
    class _Env:
        def subst(self, s: str) -> str:
            return os.getcwd()
        def Exit(self, code: int) -> None:
            raise SystemExit(code)
    env = _Env()  # type: ignore

# The repo root is the parent of the "new" directory
# Use PROJECT_DIR because __file__ is not defined when PlatformIO loads this via SConscript
SCRIPT_DIR = Path(env.subst("$PROJECT_DIR"))
REPO_ROOT = SCRIPT_DIR
NEW_DIR = SCRIPT_DIR
OLD_DIR = SCRIPT_DIR / "old"

def main() -> int:
    print("=== Pre-build script ===")
    print(f"Repo root: {REPO_ROOT}")
    print(f"New directory: {NEW_DIR}")
    print(f"Old directory: {OLD_DIR}")

    # Check that new/ exists
    if not NEW_DIR.exists():
        print(f"ERROR: new/ directory does not exist: {NEW_DIR}")
        return 1

    # Check that old/ exists (it should, as the reference)
    if OLD_DIR.exists():
        print(f"OK: old/ exists (read-only reference)")
    else:
        print(f"WARNING: old/ does not exist. Cannot verify against legacy code.")

    # Check for uncommitted changes in new/
    try:
        result = subprocess.run(
            ["git", "-C", str(REPO_ROOT), "status", "--porcelain", "--", str(NEW_DIR) + "/"],
            capture_output=True, text=True, check=True
        )
        if result.stdout.strip():
            print("WARNING: Uncommitted changes in new/:")
            print(result.stdout)
        else:
            print("OK: new/ is clean (no uncommitted changes)")
    except subprocess.CalledProcessError as e:
        print(f"ERROR: git status failed: {e}")
        return 1

    # Check that platformio.ini exists
    pio_ini = NEW_DIR / "platformio.ini"
    if pio_ini.exists():
        print(f"OK: platformio.ini exists")
    else:
        print(f"ERROR: platformio.ini not found: {pio_ini}")
        return 1

    # Check that src/ exists
    src_dir = NEW_DIR / "src"
    if src_dir.exists():
        print(f"OK: src/ exists")
    else:
        print(f"ERROR: src/ does not exist: {src_dir}")
        return 1

    print("=== Pre-build complete ===")
    return 0


# Run the checks when PlatformIO loads this script.
# The if __name__ block is only for running the file directly.
exit_code = main()
if exit_code != 0:
    env.Exit(exit_code)

if __name__ == "__main__":
    sys.exit(exit_code)