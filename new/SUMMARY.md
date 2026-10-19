# Yaeger Refactor — Summary

This document summarizes the refactored codebase in `new/`.

## Directory Layout

```
new/
├── platformio.ini                    # PlatformIO build configuration
├── README.md                         # Project overview
├── GLOSSARY.md                       # Domain vocabulary
├── ARCHITECTURE.md                   # Design decisions
├── MIGRATION.md                      # What changed, what was dropped
├── scripts/
│   ├── diagnostics.sh                # Build/lint/test/clang-tidy runner
│   ├── pre_build.py                  # Pre-build checks (git, files)
│   └── README.md                     # Script documentation
├── lib/
│   └── catch2/
│       ├── catch2.hpp                # Minimal Catch2 header
│       └── README.md                 # Usage notes
├── src/
│   ├── main.cpp                      # Entry point
│   ├── preferenceKeys.h              # Preference key constants
│   ├── roaster_config.h              # Pin definitions (devkit / mini)
│   ├── API/
│   │   ├── api.cpp                   # HTTP REST endpoints
│   │   └── api.h
│   ├── Command/
│   │   ├── Command.cpp               # WebSocket JSON parser
│   │   └── Command.h
│   ├── Controller/
│   │   ├── Controller.cpp            # PID controller, state machine
│   │   └── Controller.h
│   ├── Display/
│   │   ├── display.cpp               # I2C LCD polling driver
│   │   └── display.h
│   ├── Logging/
│   │   ├── logging.cpp               # Serial + WebSerial
│   │   └── logging.h
│   ├── Pwm/
│   │   ├── Pwm.cpp                   # PWM fan/heater driver
│   │   └── Pwm.h
│   ├── Sensor/
│   │   ├── Sensor.cpp                # MAX31855 reading + fault handling
│   │   └── Sensor.h
│   ├── vendor/
│   │   ├── AutoTunePID.cpp           # PID with autotuning (vendor)
│   │   ├── AutoTunePID.h
│   │   └── ESP32_EnhancedPWM.h       # PWM driver wrapper (vendor)
│   ├── WiFiManager/
│   │   ├── WiFiManager.cpp           # AP→STA fallback, credentials
│   │   └── WiFiManager.h
│   └── WiFiSetup/
│       ├── wifi_setup.cpp            # WiFi setup (legacy compatibility)
│       └── wifi_setup.h
└── test/
    ├── README.md                     # Test documentation
    ├── unit.cpp                      # Catch2 unit tests (PID, PWM, sensor)
    ├── integration.cpp               # Catch2 integration tests
    ├── doctest_integration.cpp       # doctest alternative
    └── integration_main.cpp          # doctest entry point
```

## Files by Purpose

### Build & Configuration

| File | Purpose |
|------|---------|
| `platformio.ini` | PlatformIO environment definitions (`esp32-s3`, `esp32-s3-mini`, `test`) |
| `scripts/pre_build.py` | Pre-build validation (git status, file existence) |
| `scripts/diagnostics.sh` | Runs `pio run --target build`, `lint`, `test`, `clang-tidy` |
| `lib/catch2/catch2.hpp` | Minimal Catch2 header (no external dependency) |

### Core Application

| File | Purpose |
|------|---------|
| `src/main.cpp` | Entry point — wires all subsystems together |
| `src/Controller/Controller.h/cpp` | PID controller with state machine (`Idle`→`Heating`→`Cooling`→`Faulted`→`Tuning`) |
| `src/Sensor/Sensor.h/cpp` | MAX31855 thermocouple reading, fault detection, moving-average filter |
| `src/Pwm/Pwm.h/cpp` | PWM fan/heater driver wrapping `ESP32_EnhancedPWM` |
| `src/WiFiManager/WiFiManager.h/cpp` | WiFi AP→STA fallback, credential persistence, MDNS |
| `src/WiFiSetup/wifi_setup.cpp/h` | Legacy WiFi setup (kept for compatibility) |
| `src/API/api.cpp/h` | HTTP REST API (`/api/wifi`, `/api/pid`, `/api/target`, `/api/fan`) |
| `src/Command/Command.h/cpp` | WebSocket JSON command parser and dispatcher |
| `src/Display/display.h/cpp` | I2C LCD polling driver (pushed by `Controller`) |
| `src/Logging/logging.h/cpp` | Unified `Serial` + `WebSerial` logging |
| `src/vendor/...` | Vendor modules (`AutoTunePID`, `ESP32_EnhancedPWM`) |

### Testing

| File | Purpose |
|------|---------|
| `test/unit.cpp` | Catch2 unit tests (PID output bounds, setpoint clamping, PWM clamping, sensor filtering, autotune modes) |
| `test/integration.cpp` | Catch2 integration tests (full control loop, fault handling, PWM bounds, WiFi state) |
| `test/integration_main.cpp` | doctest entry point (alternative test harness) |
| `test/doctest_integration.cpp` | doctest-style integration tests |

### Documentation

| File | Purpose |
|------|---------|
| `README.md` | Project overview, quick start, build targets |
| `GLOSSARY.md` | Domain vocabulary (BT, ET, MAX, PID, etc.) |
| `ARCHITECTURE.md` | Module responsibilities, design decisions, trade-offs |
| `MIGRATION.md` | What changed, what was dropped, why |

## Build Targets

| Target | Board | Purpose |
|--------|-------|---------|
| `esp32-s3` | ESP32-S3 DevKitC1 (16MB) | Main firmware |
| `esp32-s3-mini` | LoLin S3 Mini | Compact variant |
| `test` | ESP32-S3 DevKitC1 | Unit + integration tests |

## Running

```bash
# Build and flash
pio run -e esp32-s3 --target upload

# Run tests
pio run -e test --target test

# Run diagnostics (build + lint + test + clang-tidy)
./scripts/diagnostics.sh
```

## Design Decisions

- **C++23** — `std::expected`-style error handling, `std::span`, `std::format`.
- **Event-driven cooperative scheduler** — all modules run in `loop()`; no threads needed.
- **State machine** for `Controller` — explicit states make behavior predictable.
- **Sensor faults** → alert → attempt recovery → after 3 failures, shut down heater.
- **Autotune** runs non-blockingly; gains are persisted to `Preferences`.
- **Display** is polled every 50ms by `Controller`; no race conditions.
- **Vendor code** (`AutoTunePID`, `ESP32_EnhancedPWM`) is copied into `new/src/vendor/` with `VENDOR` guards.

## What Was Dropped

| Feature | Reason |
|---------|--------|
| `LiquidCrystal_I2C` fork | Unmaintained, known issues |
| Custom `MovingAverageFilter` | Simple EWMA in `Sensor` was sufficient |
| `setupSimulation()` functions | Not needed in production |

## What Was Added

- **`platformio.ini`** with explicit `[env:test]` target
- **`scripts/diagnostics.sh`** for CI-style validation
- **`scripts/pre_build.py`** for pre-flight checks
- **Unit + integration tests** with Catch2 and doctest
- **`ARCHITECTURE.md`**, `MIGRATION.md`, `GLOSSARY.md`
- **Minimal `catch2.hpp`** header (no external dependency)

---

*Generated by the refactor process. Last updated: 2026-10-19.*
