# Yaeger Refactor — README

This directory contains the **refactored** C++23 rewrite of the Yaeger coffee roaster controller.

## Quick Start

```bash
cd new
pio run -e esp32-s3 --target upload
```

## Build Targets

| Target | Board | Purpose |
|--------|-------|---------|
| `esp32-s3` | ESP32-S3 DevKitC1 (16MB) | Main firmware |
| `esp32-s3-mini` | LoLin S3 Mini | Compact variant |
| `test` | ESP32-S3 DevKitC1 | Unit + integration tests |

## Directory Layout

```
yaeger/
├── old/                 ← legacy code (read-only, DO NOT MODIFY)
├── new/                 ← this refactor
│   ├── src/            ← refactored source
│   ├── test/           ← Catch2 + doctest suite
│   ├── web/            ← Vite frontend build output
│   ├── scripts/        ← build helpers
│   ├── platformio.ini  ← PlatformIO build config
│   ├── GLOSSARY.md
│   ├── ARCHITECTURE.md
│   └── MIGRATION.md
├── GLOSSARY.md         ← shared vocabulary
├── ARCHITECTURE.md     ← design decisions
└── MIGRATION.md        ← what changed, why, what was dropped
```

## Running Diagnostics

```bash
cd new
./scripts/diagnostics.sh
```

## Notes

- The `old/` directory is a **read-only** reference. Never edit files there.
- The `new/` directory is a clean slate. It does not include the `miniweb/` frontend — that is built separately via `npm run build` and placed in `new/data/`.
- The refactor uses C++23 features (`std::expected`, `std::span`, `std::format`).

## Contributing

- Edit only files under `new/`.
- Never modify `old/`.
- Run `./scripts/diagnostics.sh` before committing.
