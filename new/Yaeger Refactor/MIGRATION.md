# MIGRATION

## From `old/` to `new/`

This document records what changed, what was dropped, and why.

## Dropped Features

| Feature | Status | Reason |
|---------|--------|--------|
| `LiquidCrystal_I2C` fork | Replaced with Adafruit's official fork | The fork had known issues and was unmaintained |
| Custom `MovingAverageFilter` | Replaced with a simple EWMA in `Sensor::takeReading()` | The custom filter was essentially a 5-point EMA; the existing logic was sufficient |
| `setupSimulation()` functions | Removed | These were for local development testing and are not needed in production |

## Architecture Changes

| Old | New |
|-----|-----|
| Monolithic `Control` class (200+ lines) | Split into `Controller` (PID + state machine) and separate modules |
| Raw `ESP32_EnhancedPWM` used directly | Wrapped in `PwmOutput` class for abstraction |
| Sensor reading logic mixed with display | `Sensor` module encapsulates reading and fault handling |
| No error handling | `std::expected`-style error reporting via `logf()` |
| No test infrastructure | Catch2 unit tests + `scripts/diagnostics.sh` |

## Build Differences

| Old (`old/`) | New (`new/`) |
|---------------|---------------|
| `lib_ignore` used to avoid conflicts | No `lib_ignore` needed — dependencies are explicit |
| Manual `pio run -t erase` in shell script | `pio run -t erase` is optional; default is incremental build |
| No test target | `pio run --target test` builds the test harness |

## Migration Notes

- The `old/` directory is **read-only**. Do not modify files in `old/`.
- The `new/` directory is a clean slate. It does not include the `miniweb/` frontend build; that is built separately via `npm run build` in the `miniweb/` directory.
- The `new/` build is compatible with the same hardware (ESP32-S3 devkit or S3-mini) and the same LittleFS layout.

## Backward Compatibility

The new code is **not** binary-compatible with the old firmware. You must flash the new firmware using the standard PlatformIO workflow:

```bash
./build_and_flash.sh s3
```

## Verification

After flashing:
1. Verify the device boots to the access point (`yaeger.local`).
2. Connect via HTTP and confirm the WebSocket status endpoint returns `{"type":"status",...}`.
3. Run the diagnostic script: `pio run --target lint && pio run --target test`.
