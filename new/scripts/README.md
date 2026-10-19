# scripts/

This directory contains pre/post build scripts.

## pre_build.py

Run before the PlatformIO build. Checks:
- `new/` directory exists
- `platformio.ini` exists
- `src/` directory exists
- No uncommitted changes in `new/`

## diagnostics.sh

Run after a build to verify:
- Lint (clang-tidy / cppcheck)
- Unit tests (Catch2)
- Integration tests (doctest)
- Static analysis

Usage:
```bash
./scripts/diagnostics.sh
```
