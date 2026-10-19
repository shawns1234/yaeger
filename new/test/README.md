# Unit Tests

This directory contains Catch2 unit tests for Yaeger.

## Test Targets

| Target | Purpose |
|--------|---------|
| `test_build` | Compiles the test suite |
| `test_run` | Runs all tests |
| `test_unit` | Runs unit tests only |

## Test Coverage

- **Controller**: PID computation, state transitions, fault handling, autotune
- **Sensor**: MAX31855 reading, fault detection, filtering
- **Pwm**: Duty cycle computation, clamping
- **WiFiManager**: Connection state transitions, AP fallback
- **Command**: JSON parsing, command dispatch

## Running Tests

```bash
pio run -e esp32-s3 --target test
```

## Writing Tests

Use Catch2's `REQUIRE` / `CHECK` macros. For timing-dependent tests, use `CHECK_NOT_EQUAL` with a tolerance.
