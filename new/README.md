# Yaeger — Coffee Roaster Controller

A clean, C++23 refactor of the Yaeger embedded coffee roaster controller.

## Quick Start

```bash
./build_and_flash.sh s3
```

## Build Targets

| Target | Board |
|--------|-------|
| `esp32-s3` | ESP32-S3 DevKitC1 (16MB) |
| `esp32-s3-mini` | LoLin S3 Mini |
| `test` | Unit + integration tests |

## Testing

```bash
pio run -e test --target build
pio run -e test --target test
```

## Diagnostics

```bash
./scripts/diagnostics.sh
```

## Architecture

See [`ARCHITECTURE.md`](ARCHITECTURE.md).

## Glossary

See [`GLOSSARY.md`](GLOSSARY.md).

## Migration

See [`MIGRATION.md`](MIGRATION.md).
