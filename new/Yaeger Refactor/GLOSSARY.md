# GLOSSARY

## Domain Vocabulary

| Term | Meaning |
|------|---------|
| BT | Bean temperature — the temperature of the coffee beans being roasted |
| ET | Exhaust temperature — the temperature of the air exiting the roaster |
| MAX | Maximum of BT and ET |
| PID | Proportional-Integral-Derivative controller — the control algorithm used to regulate temperature |
| setpoint | The target temperature the controller attempts to reach |
| heater | The SSR controlling power to the heating element |
| fan | The PWM-controlled fan that cools the roaster |
| autotune | Automatic PID tuning performed by the controller |
| manual mode | Direct control of heater and fan via the UI |
| tuning mode | The autotune process that determines optimal PID gains |
| fault | A sensor error condition (open or short circuit) |
| recovery | An attempt to recover from a fault by resetting the sensor |
| shutdown | The fail-safe state where the heater is turned off after failed recovery attempts |

## Technical Terms

| Term | Meaning |
|------|---------|
| ESP32-S3 | The microcontroller platform used for Yaeger |
| MAX31855 | The thermocouple-to-thermistor bridge chip |
| PWM | Pulse-width modulation used for fan/heater control |
| I2C | The serial bus used for the OLED display |
| LittleFS | The embedded filesystem used for static assets |
| OTA | Over-the-air firmware update |
| WebSocket | The protocol used for real-time bidirectional communication |
| AP → STA | Access Point to Station mode — the fallback behavior when Wi-Fi fails |

## Project-Specific Terms

| Term | Meaning |
|------|---------|
| Yaeger | The embedded coffee roaster controller |
| Reyaeger | The modern web frontend (Vite + vanilla JS) |
| Classic | The original frontend (still supported) |
| old/ | The legacy, unmaintained codebase (read-only) |
| new/ | The refactored codebase |

## Notes

- Temperature values are always in degrees Celsius (°C).
- Heater power is expressed as a percentage (0–100%).
- Fan speed is expressed as a percentage (0–100%).
- PID gains are stored as: Kp, Ki, Kd.
