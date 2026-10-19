---
name: tdd
description: "Test-driven development workflow for C++/Arduino projects."
tags: ["testing", "c++", "arduino", "platformio", "catch2", "doctest"]
related_skills: ["home-lab-ops", "diagnosing-bugs", "grilling"]
---

# TDD (Test-Driven Development)

A repeatable workflow for building embedded C++ code on ESP32/Arduino with Catch2 + doctest.

## When to use

- You are adding new functionality to an existing embedded project.
- You are refactoring existing code and want regression tests.
- You need to document expected behavior through tests.
- You are working on a new module and want a test harness before the full build.

## Workflow

### Step 1: Write a failing test

1. Create a test file under `test/<module>_test.cpp` (or `test/` at the repo root).
2. Write a test case that exercises the new behavior.
3. Run the test with `pio run -e test --target test --filter "<test_name>"`.
4. The test MUST fail (red). If it passes, you missed the point.

### Step 2: Write the minimal implementation

1. Implement JUST enough code to make the test pass.
2. Keep the implementation simple and focused on the test.
3. Run the test again — it should now pass (green).

### Step 3: Refactor

1. Write a new, more robust test that covers a different case.
2. Refactor the implementation to handle the new case.
3. Re-run all tests — none should break.
4. Repeat until the code is clean and well-tested.

### Step 4: Run the full suite

```bash
pio run -e test --target test
```

## Test file naming

- `test/unit/<module>_test.cpp` — unit tests for a single module.
- `test/integration_test.cpp` — integration tests that exercise multiple modules.
- `test/<project>_test.cpp` — top-level tests that run the full application.

## Catch2 macros

```cpp
TEST_CASE("module: description", "[module]") {
    // Arrange
    // Act
    // Assert
}
```

Use `REQUIRE`, `CHECK`, `CHECK_THROWS_WITH`, etc. as appropriate.

## doctest for integration tests

When you need to run the full application (with `main.cpp`), use doctest:

```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("integration: full system") {
    // Set up the full application
    // Call main() or simulate the full flow
    // Assert the observable behavior
}
```

## PlatformIO test target

Add a `[env:test]` section to `platformio.ini`:

```ini
[env:test]
extends = core
board = esp32-s3-devkitc1-n16r8
build_flags =
    -D CORE_DEBUG_LEVEL=5
    -D BOARD_HAS_PSRAM
    -D ARDUINO_USB_CDC_ON_BOOT=1
    -D ARDUINO_USB_MODE=1
lib_deps =
    ESP32Async/ESPAsyncWebServer
    ArduinoJson
    Catch2
```

Then run:

```bash
pio run -e test --target test
```

## Tips and pitfalls

- **Write the test BEFORE the implementation.** If the test passes on the first try, you haven't learned anything.
- **Keep tests fast.** Each test should run in under 100ms. Avoid blocking on hardware or the network.
- **Mock hardware dependencies.** Don't test the real thermocouple or SSR in unit tests. Mock them or stub the driver calls.
- **Use `CHECK_THROWS_WITH`** when testing error paths.
- **Use `CHECK_NOT_EQUAL`** for timing-dependent assertions — compare against a threshold, not an exact value.
- **Run tests in isolation.** Each test should be able to run independently without depending on global state.
- **Don't mix up Catch2 and doctest.** Use Catch2 for unit tests and doctest for integration tests.

## Example

```cpp
TEST_CASE("Sensor: filtered value is a moving average", "[sensor]") {
    Sensor sensor(33, 37, 35, "Test");
    sensor.takeReading();

    float v = sensor.getFilteredValue();
    CHECK(v >= -100.0f);
    CHECK(v <= 200.0f);
}
```

## When not to use TDD

- Prototyping a proof-of-concept where the API is not stable.
- Very small functions (1–2 lines) where the test overhead outweighs the benefit.
- Exploratory work where the API is still changing.

In those cases, write tests as you go, but don't make the test the first step.
