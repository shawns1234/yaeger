# ARCHITECTURE

## Overview

Yaeger is an embedded controller for a coffee bean roaster, running on an ESP32-S3. It reads two thermocouple sensors (bean and exhaust temperatures), controls a PWM fan and an SSR-driven heating element, and exposes a WebSocket API for a web frontend.

## High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Yaeger Controller                         │
├─────────────────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐  │
│  │   Controller │  │   Sensor     │  │     WiFiManager   │  │
│  │  (PID + SM)  │←─│  (MAX31855)  │  │   (AP↔STA)       │  │
│  └──────────────┘  └──────────────┘  └──────────────────┘  │
│         │                              │                      │
│         ▼                              ▼                      │
│  ┌──────────────┐  ┌─────────────────────────────────────┐  │
│  │   Command    │  │         AsyncWebServer               │  │
│  │  (WS parser) │  │  ┌──────────────┐  ┌──────────────┐ │  │
│  └──────────────┘  │  │  /api/wifi   │  │  /api/pid    │ │  │
│                     │  │  /api/target │  │  /api/fan    │ │  │
│                     │  │  /api/status │  └──────────────┘ │  │
│                     │  └──────────────┘                    │  │
│                     │                                      │  │
│                     └─────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

## Modules

| Module | Responsibility |
|--------|----------------|
| `Controller` | PID computation, state machine, actuator driving |
| `Sensor` | MAX31855 reading, fault detection, filtering |
| `Actuator` | PWM fan and heater drivers (wraps `ESP32_EnhancedPWM`) |
| `WiFiManager` | AP→STA fallback, MDNS, credential persistence |
| `Command` | WebSocket command parsing and dispatch |
| `API` | HTTP REST endpoints |
| `Display` | I2C LCD polling driver |
| `Logging` | Serial + WebSerial unified logging |

## Design Decisions

### Controller as a State Machine

The controller uses a discrete state machine with states: `Idle`, `Heating`, `Cooling`, `Faulted`, `Tuning`. This is simpler than an event-driven approach for a single-threaded `loop()` model, and it makes the control logic easier to reason about.

### Sensor Fault Handling

On a `MAX31855_FAULT_*` error:
1. Log the fault with a human-readable message.
2. Attempt recovery (reset the sensor).
3. If recovery fails 3 times, enter `Faulted` state and shut off the heater.

### PID Autotune

Autotune runs in a dedicated thread (non-blocking) that toggles the heater every 2 seconds. After the specified number of oscillations, it computes `Ku` and `Tu` and applies a chosen tuning rule (default: Ziegler-Nichols). The UI exposes both auto-tuned and manually-set gains.

### Display Polling

The LCD is driven by a polling loop that reads the controller's state every 50ms and updates the display. This is simpler than a push model and avoids race conditions between the controller loop and the display driver.

### API Design

The WebSocket API uses a simple JSON command format: `{"command":"setHeater","value":60}`. Each command is validated and returns a 4xx/5xx response on error. The HTTP API is a thin layer that persists preferences to `Preferences`.

### Vendor Modules

`AutoTunePID` and `ESP32_EnhancedPWM` are copied into `new/src/vendor/` with a `VENDOR` guard. They are not recompiled — the precompiled binaries from PlatformIO are linked. This keeps the build fast and avoids re-licensing issues.

## Build Order

1. Build LittleFS (static assets)
2. Upload LittleFS
3. Build firmware
4. Upload firmware

This order ensures the firmware has a filesystem to serve.
