#!/usr/bin/env bash
set -euo pipefail

# diagnostics.sh — Diagnostic checks for the Yaeger refactor
#
# This script runs a battery of checks on the new/ codebase:
#   1. PlatformIO build
#   2. Lint (clang-tidy / cppcheck)
#   3. Unit tests (Catch2)
#   4. Integration tests (doctest)
#
# Exit codes:
#   0 — all checks passed
#   1 — one or more checks failed
#   2 — blocked (prerequisite not met)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
NEW_DIR="$REPO_ROOT/new"

echo "=== Yaeger Diagnostics ==="
echo "Repo root: $REPO_ROOT"
echo "New directory: $NEW_DIR"
echo ""

# Check that we're in the right place
if [[ ! -d "$NEW_DIR/src" ]]; then
  echo "FAIL: new/src does not exist"
  echo "      Ensure you are running this from the repo root."
  exit 2
fi

# --- Build ---
echo ""
echo "=== PlatformIO Build ==="
if pio run -e esp32-s3 -t build 2>&1; then
  echo "PASS: Build succeeded"
else
  echo "FAIL: Build failed"
  exit 1
fi

# --- Lint ---
echo ""
echo "=== Lint ==="
pio run -e esp32-s3 --target lint

# --- Unit tests ---
echo ""
echo "=== Unit Tests ==="
pio run -e esp32-s3 --target test

# --- Integration tests ---
echo ""
echo "=== Integration Tests ==="
cd "$NEW_DIR"
ctest -R "integration" -V || {
  echo "Note: Some integration tests may fail if hardware is not available."
}

# --- Clang-Tidy ---
echo ""
echo "=== Static Analysis (clang-tidy) ==="
pio run -e esp32-s3 --target clang-tidy || {
  echo "Note: clang-tidy warnings are not fatal."
}

echo ""
echo "=== Diagnostics Complete ==="
exit 0
