# PotatoMelt-ESP32 Constitution

<!--
===================================================================================
SYNC IMPACT REPORT - Constitution Version 1.2.0
===================================================================================
Version Change: 1.1.0 → 1.2.0
Modified Principles:
  - IX. Persistent Configuration (NEW) - Added requirements for NVS storage of
    runtime-tunable parameters and calibration data
  - X. Code Organization (NEW) - Established project structure conventions for
    libraries, subsystems, and separation of interface/implementation
  - VII. Combat Robot Domain Standards (UPDATED) - Added RPM calculation methodology
Added Sections:
  - IX. Persistent Configuration
  - X. Code Organization
Removed Sections: N/A
Templates Requiring Updates:
  ✅ .specify/templates/plan-template.md (validated - compatible)
  ✅ .specify/templates/spec-template.md (validated - compatible)
  ✅ .specify/templates/tasks-template.md (validated - compatible)
Follow-up TODOs: None - all sections complete
===================================================================================
-->

## Core Principles

### I. Performance-First Architecture

**Rule**: Code execution time is CRITICAL. Performance MUST be prioritized over abstraction.

**Requirements**:
- Direct hardware register access MUST be used whenever possible over library abstractions
- Minimize function call overhead in time-critical loops (hotloop, spin control, motor control)
- Prefer inline functions and macros for frequently-called operations
- Use compile-time constants and constexpr wherever applicable
- Avoid dynamic memory allocation in real-time loops
- Profile and measure timing-critical code paths
- Document execution time budgets for critical sections

**Rationale**: Combat robots operate in real-time environments where microseconds matter.
The melty brain algorithm requires precise timing to maintain heading control at high RPM
(up to 3000 RPM). Motor control, IMU sampling, and LED timing must execute with minimal
latency to ensure safe and effective operation.

### II. Hardware Integration Discipline

**Rule**: Hardware peripherals MUST be configured and managed with explicit control.

**Requirements**:
- Pin assignments MUST be centralized in `melty_config.h`
- Hardware configuration (I2C, RMT, ADC) MUST use ESP32 native APIs for maximum control
- All hardware timing parameters MUST be documented with physical units (Hz, ms, µs)
- Hardware-dependent constants MUST be clearly marked (e.g., `ACCELEROMETER_HARDWARE_RADIUS_CM`)
- Register-level access MUST be preferred for performance-critical peripherals
- Hardware initialization sequences MUST be explicit and commented
- Pin state and peripheral state MUST be deterministic on startup

**Rationale**: ESP32-S3 embedded systems require precise hardware control. Direct
register access provides maximum performance and deterministic behavior. Centralized
configuration prevents conflicts and enables rapid hardware iteration.

### III. Real-Time Safety Critical

**Rule**: Code MUST implement multiple layers of safety mechanisms. Human safety is paramount.

**Requirements**:
- Controller failsafe MUST be implemented (connection timeout, alive check, dead-man switch)
- Battery monitoring MUST prevent over-discharge that could cause fires
- Watchdog timers MUST be respected (explicit task yields required)
- State machines MUST have well-defined failure modes
- Motor control MUST default to safe state (stopped) on ANY error condition
- Critical parameters MUST have compile-time sanity checks where possible
- Safety timeouts MUST be clearly documented (e.g., `CONTROL_UPDATE_TIMEOUT_MS`)

**Rationale**: Combat robots are spinning masses with brushless motors. A software failure
can cause physical harm to operators or spectators. The code controls a weapon system that
must fail safely under all circumstances.

### IV. Documentation Excellence

**Rule**: Code documentation is MANDATORY and MUST explain intent, constraints, and safety implications.

**Requirements**:
- Every configuration constant MUST have an inline comment explaining purpose and units
- Complex algorithms MUST have block comments explaining theory and approach
- Safety-critical sections MUST be marked with clear warnings
- Hardware-specific behaviors MUST be documented (e.g., "forward on xbox gives negative values")
- Magic numbers MUST be eliminated or explained
- Public API functions MUST have header comments describing parameters, return values, and side effects

**Rationale**: Embedded systems code is difficult to debug and test. Combat robots have
long iteration cycles (build, test, fix). Excellent documentation reduces development
time and prevents dangerous misconfigurations. This code will be extended and modified
for custom robot implementations.

### V. Embedded Systems Best Practices

**Rule**: Code MUST follow ESP32 and FreeRTOS conventions and constraints.

**Requirements**:
- Use dual-core architecture appropriately (hotloop on core 0, control on core 1)
- Task priorities MUST be documented and justified
- Stack sizes MUST be explicitly specified (not defaulted)
- Inter-task communication MUST use thread-safe mechanisms
- Shared state MUST be protected or designed for lock-free access
- ISR-safe functions MUST be marked and used correctly
- Flash vs IRAM placement MUST be considered for time-critical code
- Memory usage MUST be monitored (stack, heap, static)

**Rationale**: ESP32 is a dual-core embedded platform with specific constraints. FreeRTOS
task scheduling must be understood to achieve real-time performance. Incorrect usage
can cause watchdog resets, race conditions, or unpredictable behavior.

### VI. Modularity & Extensibility

**Rule**: Code MUST be structured to enable customization while preserving the base implementation.

**Requirements**:
- Subsystems MUST be isolated into separate modules (battery, IMU, LED, storage)
- Configuration MUST be centralized and easily modified for different robots
- Hardware abstractions MUST support different sensor configurations (dual accelerometers)
- Control parameters MUST be tunable at runtime where appropriate (RPM, trim, translation)
- New features MUST be additive, not require core rewrites
- Version-specific configurations MUST be clearly marked

**Rationale**: This is a base codebase for building custom 3lb melty brain combat robots.
Different builders will have different hardware configurations, weight classes, and
feature requirements. The architecture must support extension without breaking the
proven core functionality.

### VII. Combat Robot Domain Standards

**Rule**: Code MUST adhere to combat robotics community standards and constraints.

**Requirements**:
- DShot protocol MUST be used for ESC communication (standard in combat robotics)
- PID control loops MUST be tunable for different robot masses and geometries
- Heading LED timing MUST be configurable for different RPM ranges
- Battery cell chemistry MUST be configurable (different voltage curves)
- Safety features MUST match competition requirements (e.g., failsafe behavior)
- Motor reversing MUST be supported for bidirectional drive
- Telemetry MUST be available via Serial for debugging and tuning
- RPM MUST be calculated from centripetal acceleration using dual accelerometers
- Per-RPM calibration correction factors MUST be supported for accurate heading control

**Rationale**: Combat robots compete in standardized events with specific rules and
technical requirements. The code must integrate with standard hobby electronics
(brushless motors, ESCs, controllers) and support common competitive configurations.

### VIII. Testing Philosophy

**Rule**: Automated unit and integration tests are NOT REQUIRED for this project.

**Requirements**:
- Hardware-in-the-loop testing on physical robot is the primary validation method
- Code validation MUST occur through actual robot operation
- No automated test suites, test frameworks, or test files are needed
- Safety validation through physical testing is mandatory
- Serial logging and telemetry provide runtime validation
- Configuration validation through iterative hardware testing

**Rationale**: This is embedded hardware control code for a physical combat robot.
The true test is whether the robot operates safely and correctly in the real world.
Mocking hardware behaviors introduces complexity without adding value. The tight
integration between hardware timing, sensor feedback, and motor control makes
automated testing impractical and potentially misleading. Real-world testing with
incremental validation (tank mode → low RPM → high RPM) is more effective and
reliable than any test suite could be.

### IX. Persistent Configuration

**Rule**: Runtime-tunable parameters MUST persist across power cycles using ESP32 NVS.

**Requirements**:
- ESP32 Preferences API MUST be used for persistent storage
- Configuration namespace MUST be clearly identified (e.g., "potatomelt")
- Per-RPM calibration data MUST be stored with descriptive key patterns (e.g., `a_cor_[rpm]`)
- Default values MUST be provided for all stored parameters
- Storage keys MUST be documented and consistent
- Calibration adjustments made during operation MUST be automatically persisted
- Read/write operations MAY include commented-out debug logging for troubleshooting

**Rationale**: Combat robots require field calibration and tuning. Each robot has unique
physical characteristics (mass distribution, sensor mounting, motor characteristics)
that require per-RPM correction factors. Operators adjust these during testing sessions,
and they must persist between power cycles. The ESP32 NVS provides reliable non-volatile
storage that survives reboots, allowing robots to "remember" their calibration state.

### X. Code Organization

**Rule**: Code MUST follow consistent organizational patterns for libraries, subsystems, and modules.

**Requirements**:
- Custom hardware libraries MUST be placed in `src/lib/` directory
- Subsystem modules MUST be placed in `src/subsystems/` directory
- Each module MUST separate interface (.h) from implementation (.cpp)
- Header guards MUST use `#ifndef`/`#define` pattern (not `#pragma once`)
- Float literals MUST use explicit `f` suffix (e.g., `5.13f`, `2.0f`)
- Constants SHOULD prefer `constexpr` over `#define` where type safety is beneficial
- Public interfaces MUST be declared in header files
- Implementation details MUST remain in .cpp files

**Rationale**: Consistent organization makes the codebase navigable and maintainable.
Separating libraries from subsystems clarifies whether code is hardware-specific or
application logic. The .h/.cpp separation enables faster compilation and clearer
interfaces. Following ESP32/Arduino conventions ensures compatibility with the
toolchain and broader ecosystem.

## Performance Standards

**Execution Time Budgets**:
- Hotloop iteration MUST complete within 250µs (4kHz update rate target)
- Spin control calculation MUST complete within 100µs per rotation
- Motor command transmission MUST complete within 50µs
- IMU sampling MUST complete within 20µs
- LED update MUST complete within 30µs

**Memory Constraints**:
- No dynamic allocation in time-critical paths

**Real-Time Guarantees**:
- Controller timeout threshold: 3000ms (configurable in `melty_config.h`)
- Maximum rotation tracking interval: Based on MIN_TRACKING_RPM
- Watchdog must be serviced every loop iteration

## Code Quality Standards

**Compilation**:
- Code MUST compile without warnings on Arduino IDE for ESP32-S3
- All `#define` constants MUST have type-safe values where possible
- Header guards MUST be used in all `.h` files

**Testing** (Hardware Validation Only):
- Automated test suites are NOT required or expected
- New features MUST be tested on physical hardware before merge
- Safety features MUST be explicitly tested on physical hardware (failsafe, battery cutoff)
- Configuration changes MUST be validated across different robot configurations
- Performance-critical changes MUST be profiled using real hardware telemetry

**Version Control**:
- Configuration changes MUST document which robot variant they apply to
- Breaking changes MUST be clearly marked in commit messages
- Hardware-specific code MUST be marked with comments

## Technology Stack

**Platform**: ESP32-S3 (dual-core Xtensa LX7, FreeRTOS)

**Primary Dependencies**:
- Arduino framework for ESP32
- Bluepad32 (Bluetooth game controller interface)
- PID_v1 (PID control library)
- Wire (I2C communication)
- ESP32 RMT (Remote Control Transceiver for DShot)

**Hardware Interfaces**:
- I2C: Dual LIS331 accelerometers (0x18, 0x19)
- RMT: DShot motor control (channels 1, 2) + NeoPixel LED (channel 0)
- ADC: Battery voltage monitoring
- Bluetooth: Xbox controller input via Bluepad32

**Testing Approach**:
- Hardware-in-the-loop testing (physical robot) - PRIMARY METHOD
- No automated unit or integration tests required
- Serial console logging for debugging and validation
- Incremental testing: tank mode → low RPM spin → high RPM spin
- Safety validation before each test session
- Telemetry-based validation during operation

## Governance

**Amendment Process**:
1. Proposed changes MUST be documented with justification
2. Impact on existing code and robots MUST be assessed
3. Safety implications MUST be reviewed
4. Version number MUST be incremented per semantic versioning:
   - MAJOR: Safety-critical changes, API breaks, removed features
   - MINOR: New principles added, new safety requirements
   - PATCH: Clarifications, documentation improvements

**Compliance Verification**:
- All code changes MUST be checked against this constitution
- Performance-critical code MUST document execution time measurements
- Safety-critical code MUST be explicitly reviewed
- Configuration changes MUST verify compatibility with base code

**Version Control**:
- This constitution applies to the PotatoMelt-ESP32 repository
- Derived projects MAY extend this constitution but MUST NOT weaken safety requirements
- All versions MUST maintain compatibility with LGPL 2.1 license

**Development Guidance**:
- This constitution is the authoritative governance document
- Template files in `.specify/templates/` provide workflow guidance
- README.md contains user-facing documentation
- TODOs in code represent known extension points

**Version**: 1.2.0 | **Ratified**: 2025-10-27 | **Last Amended**: 2025-10-27
