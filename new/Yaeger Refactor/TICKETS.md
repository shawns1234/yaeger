# Yaeger Refactor — Ticket Tracker

> A simple Markdown-based issue tracker for the Yaeger refactor.
>
> Open this in GitHub or any Markdown viewer. It doubles as the "map" for the wayfinder skill.

---

## 📋 Ticket Index

| # | Title | Status |
|---|-------|--------|
| 1 | Build & flash on ESP32-S3 DevKitC1 | ✅ Done |
| 2 | Run `scripts/diagnostics.sh` and fix any failures | 🟡 In Progress |
| 3 | Verify WebSocket API (`/ws`) returns correct telemetry | ⚪ Open |
| 4 | Verify HTTP API (`/api/wifi`, `/api/pid`, `/api/target`, `/api/fan`) | ⚪ Open |
| 5 | Implement I2C LCD display driver (polling) | ⚪ Open |
| 6 | Implement OTA manifest with rollback support | ⚪ Open |
| 7 | Set up CI workflow (GitHub Actions) | ⚪ Open |
| 8 | Write integration tests for full control loop | ⚪ Open |
| 9 | Update `README.md` with build instructions | ⚪ Open |
| 10 | Document sensor fault recovery behavior in `ARCHITECTURE.md` | ⚪ Open |

---

## 📝 Notes

- **Session handoff:** See `HANDOFF.md` in the repo root for session continuity.
- **Build:** `pio run -e esp32-s3 --target upload`
- **Test:** `pio run -e test --target test`
- **Diagnostics:** `./scripts/diagnostics.sh`

---

## 🔍 Open Issues

### 3. WebSocket API telemetry

- Verify `/ws` broadcasts status updates every ~100ms.
- Verify fields: `ET`, `BT`, `Amb`, `BurnerVal`, `Setpoint`, `Target`, `Mode`, `FanVal`.

### 4. HTTP API

- Verify `/api/wifi` POST saves credentials.
- Verify `/api/pid` GET/POST works.
- Verify `/api/target` POST sets target.
- Verify `/api/fan` POST sets fan speed.

### 5. I2C LCD display

- Implement polling driver that pushes state every 50ms.
- Handle I2C timeout gracefully (don't block the main loop).

### 6. OTA manifest

- Define manifest schema in LittleFS.
- Implement rollback on boot if manifest is missing or corrupt.

### 7. CI workflow

- GitHub Actions: build + test on push to `main`.
- Lint with `cppcheck` and `clang-tidy`.

### 8. Integration tests

- Full control loop: setpoint → PID → actuator → sensor.
- Fault injection: simulate `MAX31855_FAULT_OPEN` and verify recovery/shutdown.

### 9. `README.md`

- Add build instructions.
- Add hardware requirements.
- Add troubleshooting section.

### 10. Documentation

- Expand `ARCHITECTURE.md` with the sensor fault recovery sequence.
- Add a section on "How to extend the controller" for community contributors.

---

## 📈 Progress

- **Completed:** Architecture design, module scaffolding, unit tests, platformio.ini, docs.
- **In Progress:** Diagnostics run, API verification.
- **Remaining:** Display driver, OTA, CI, integration tests, docs polish.

---

## 🧭 Next Steps

1. Run `./scripts/diagnostics.sh` in `new/` to see any compilation errors.
2. Fix any errors that appear.
3. Open the project in VS Code.
4. Run `pio run -e esp32-s3 --target upload` to flash.
5. Connect to `yaeger.local` (or the IP shown on boot) and verify the UI.

If you hit a blocker, open a ticket above and describe the error.
