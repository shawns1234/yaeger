# Coffee Roaster Refactor — Handoff Document

**Handoff date:** 2026-10-21

**Prepared by:** ops (Orchestrator)

---

## Board State Summary

The `roaster-refactor` Kanban board (default board) contains **18 tasks**:

### ✅ Done (5)

| ID | Title |
|---|---|
| t_ea612749 | Orchestrator — Manage the Roaster Refactor Pipeline |
| t_22aa9e36 | Remind myself to install Python 3.14 so pio.exe can run |
| t_e0aa4956 | Handoff: Verify PATH and remove any remaining old pio.exe shims |
| t_fafdba38 | Handoff: Verify new project folder is ready and platformio.ini is in place |
| t_92ff342b | Handoff: Confirm the new repo structure and ensure old/new code trees are separated |

### 🔴 Blocked (5)

| ID | Title | Blocker |
|---|---|---|
| t_f550a6a8 | Architect — C++ Refactor Lead | User requested pause. Separating old/new code trees before proceeding. |
| t_2f0abb3e | Architect — Implement the Roaster module (roaster.cpp) | Kanban owner refused: storage_unavailable |
| t_f6469c61 | Architect — Implement the Hardware module (hardware.cpp) | Kanban owner refused: storage_unavailable |
| t_1ec48e50 | Architect — Implement the Network module (network.cpp) | Kanban owner refused: storage_unavailable |
| t_9b052238 | Architect — Implement the Logging module (logging.cpp) | Kanban owner refused: storage_unavailable |

### ⏳ Todo (6)

| ID | Title |
|---|---|
| t_75ce2eed | Build-Verify — Compile & Flash |
| t_4b1bf125 | Docs — Documentation & Changelogs |
| t_5c3812bc | QA — Test Suite Runner |
| t_d8785342 | Docs — Documentation & Changelogs |
| t_baa8b9c6 | Build-Verify — Compile & Flash |
| t_5e413fd8 | QA — Test Suite Runner |

### ▶️ Running (1)

| ID | Title |
|---|---|
| t_11dfffd3 | Handoff: Ensure the Kanban board is up to date and all pending cards are documented |

---

## Board Graph (Dependencies)

```
t_ea612749 (Orchestrator, done)
├── t_f550a6a8 (Architect C++ Lead, blocked)
│   ├── t_75ce2eed (Build-Verify, todo)
│   │   └── t_5c3812bc (QA, todo)
│   └── t_4b1bf125 (Docs, todo)
├── t_b220336d (Architect C++ Lead, ready)
│   ├── t_d8785342 (Docs, todo)
│   └── t_baa8b9c6 (Build-Verify, todo)
│       └── t_5e413fd8 (QA, todo)
├── t_2f0abb3e (Architect Roaster, blocked)
├── t_f6469c61 (Architect Hardware, blocked)
├── t_1ec48e50 (Architect Network, blocked)
├── t_9b052238 (Architect Logging, blocked)
├── t_e0aa4956 (Handoff PATH, done)
├── t_fafdba38 (Handoff project folder, done)
└── t_92ff342b (Handoff repo structure, done)
```

---

## Build Toolchain

- **PlatformIO:** `pioarduino` (modified fork of PlatformIO)
- **Python:** 3.14.7 (`C:\Python314\python.exe`)
- **PIO install path:** `C:\Tools\pioarduino`
- **Environment:** `esp32-s3-devkitc1-n16r8`
- **Serial port:** Not currently configured (no device connected)
- **Known issues:**
  - A duplicate `[env:esp32-s3-mini]` section remains in `platformio.ini` (line 70) — a pre-existing issue unrelated to the refactor.
  - Build was last attempted via `pio run` but failed due to a Git submodule clone error for `nexgen_filter`.

---

## Completed Work (from `t_ea612749`)

All four module files were implemented and committed to Git:

| File | Purpose |
|---|---|
| `app/roaster/roaster.cpp` | PID-like temperature control, roast curve computation |
| `app/hardware/hardware.cpp` | GPIO/PWM/ADC/UART abstraction |
| `app/network/network.cpp` | WiFi/HTTP/MQTT/WebSocket abstraction |
| `app/logging/logging.cpp` | Multi-backend logger (Serial/UART/TCP/file/MQTT/systemd) |
| `app/main.cpp` | Entry point wiring all subsystems |
| `app/ota/ota.cpp` | Firmware update manager |

The implementation was committed to Git under the branch `coffee-roaster/t_ea612749-orchestrator-manage-the-roaster-refactor-pipeli`.

---

## Next Steps for the Receiving Session

1. **Fix the storage_unavailable error** for the four blocked Architect tasks (`t_2f0abb3e`, `t_f6469c61`, `t_1ec48e50`, `t_9b052238`). This appears to be a workspace/workspacekit availability issue.
2. **Unblock the Orchestrator** (`t_f550a6a8`) by confirming whether the user wants to proceed with the refactor or continue separating code trees.
3. **Address the Git submodule issue** for `nexgen_filter` if you want to continue building.
4. **Fix the duplicate `[env:esp32-s3-mini]` section** in `platformio.ini` if desired.

---

*End of handoff document.*
