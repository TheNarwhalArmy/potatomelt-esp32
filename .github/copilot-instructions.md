# Copilot Coding Agent Instructions for PotatoMelt-ESP32

## Project Overview
- **PotatoMelt-ESP32** is a drive controller for Meltybrain (translational drift) robots, targeting the ESP32-S3 platform.
- The codebase is organized for embedded development, with a focus on real-time control, sensor integration, and gamepad-based user input.

## Key Architecture & Components
- Main entry: `potatomelt/potatomelt.ino` (Arduino sketch)
- Core logic: `potatomelt/src/` (robot control, configuration, subsystems)
  - `controller.*`: Gamepad input, control logic
  - `robot.*`: Robot state, high-level behavior
  - `subsystems/`: Hardware abstraction (IMU, battery, LEDs, storage)
  - `lib/`: Motor control (DShot), accelerometer, and third-party libs
- Example sketches: `potatomelt/examples-temp/`

## Developer Workflows
- **Build/Flash**: Use PlatformIO or Arduino IDE for ESP32-S3. Board config and upload settings are not in this repo—refer to your local environment.
- **Debugging**: Serial output is used for runtime diagnostics. Check for `Serial.print` statements in core files.
- **Testing**: No formal test suite; validate changes by running on hardware and observing LED/status behavior.

## Project-Specific Patterns & Conventions
- **Subsystems**: Each hardware feature (IMU, battery, LED, etc.) is encapsulated in its own class under `subsystems/`.
- **Configuration**: Tunable parameters are in `melty_config.h`.
- **Gamepad Mapping**: See README and `controller.cpp` for input-to-action mapping.
- **LED Feedback**: LED color and pattern indicate system state (see README for meanings).
- **Motor Control**: Uses DShot protocol via `lib/DShotRMT.*`.
- **No RTOS**: The code is single-threaded, event-driven via the Arduino loop.

## Integration & External Dependencies
- Relies on ESP32 Arduino core and PlatformIO/Arduino build tools.
- Uses SparkFun LIS331 accelerometer library (see `lib/SparkFun_LIS331_ESP32.*`).
- DShot motor control implemented in `lib/DShotRMT.*`.

## Examples & References
- For new subsystems, follow the class structure in `subsystems/` (e.g., `led.h/cpp`).
- For new control logic, see `controller.cpp` and `robot.cpp` for integration points.

---

**Edit this file to update project-specific agent instructions.**
