# Handoff Document

> Use this file to preserve progress across sessions. Read this before starting a new work session.

---

## Current State

- **Architecture:** Complete. Modules are defined in `new/src/`.
- **Build system:** PlatformIO (`platformio.ini`) with three targets: `esp32-s3`, `esp32-s3-mini`, `test`.
- **Testing:** Catch2 unit tests in `test/unit.cpp`, integration tests in `test/integration.cpp`.
- **Documentation:** `GLOSSARY.md`, `ARCHITECTURE.md`, `MIGRATION.md`, `SUMMARY.md` exist.
- **Ticket tracker:** `TICKETS.md` in `new/` tracks remaining work.

---

## How to Pick Up Where We Left Off

### 1. Open VS Code

Open `W:\GitHub\yaeger` in VS Code.

### 2. Run the pre-build check

```bash
cd W:\GitHub\yaeger\new
python scripts/pre_build.py
```

This will report uncommitted changes and missing files.

### 3. Run diagnostics

```bash
./scripts/diagnostics.sh
```

Fix any errors reported.

### 4. Build and flash

```bash
pio run -e esp32-s3 --target build
pio run -e esp32-s3 --target upload
```

### 5. Verify

- Connect to `yaeger.local` in a browser.
- Open the WebSocket tab in the dev tools and confirm telemetry updates.

---

## Open Tickets

See `TICKETS.md` for the current list.

---

## Known Issues

- The `old/` directory is a read-only reference. Do not edit it.
- The `new/` directory is the refactor. Do not edit `old/` — edit `new/` instead.
- The `miniweb/` frontend is not yet built into the refactor. It will be built separately and copied into `new/data/`.

---

## Session Continuity

If you need to pause and come back later:

1. Commit your changes to `new/` (do not commit `old/`).
2. Push to your fork.
3. When you resume, clone your fork, checkout `main`, and follow the steps above.

---

## Questions?

- See `ARCHITECTURE.md` for design decisions.
- See `SUMMARY.md` for a file-by-file overview.
- See `TICKETS.md` for the work queue.
