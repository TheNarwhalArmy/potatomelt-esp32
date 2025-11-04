# Feature Specification: Configurable NeoPixel LED Count

**Feature Branch**: `001-configurable-neopixel`  
**Created**: October 27, 2025  
**Status**: Draft  
**Input**: User description: "modify the LED system to work with a configurable number of neopixel LEDs"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Configure LED Count at Compile Time (Priority: P1)

Robot builders can configure the number of NeoPixel LEDs their robot uses by changing a single configuration value, allowing the same codebase to support robots with different LED configurations (1 LED for simple status indication, 2 LEDs for left/right indicators, or more LEDs for enhanced visual effects).

**Why this priority**: This is the core requirement and enables all other functionality. Without this, the LED system remains hardcoded to exactly 2 LEDs and cannot support different robot configurations.

**Independent Test**: Can be fully tested by changing the LED count configuration value, compiling, and verifying that status indicators (ready state, low battery) correctly illuminate all configured LEDs.

**Acceptance Scenarios**:

1. **Given** a robot configured for 1 NeoPixel LED, **When** the robot enters ready state, **Then** the single LED displays the ready status color
2. **Given** a robot configured for 2 NeoPixel LEDs, **When** the robot enters ready state, **Then** both LEDs display the ready status color
3. **Given** a robot configured for 4 NeoPixel LEDs, **When** the robot enters ready state, **Then** all 4 LEDs display the ready status color
4. **Given** a robot configured for 8 NeoPixel LEDs, **When** the robot enters ready state, **Then** all 8 LEDs display the ready status color

---

### User Story 2 - Dynamic Status Display Across All LEDs (Priority: P2)

Robot operators can see consistent status information across all configured LEDs, with all LEDs displaying the same color/pattern for system-wide states (ready, low battery, controller disconnected) to ensure visibility regardless of robot orientation or LED count.

**Why this priority**: Once LED count is configurable, all status indication functions must work correctly with any count. This ensures the robot provides clear feedback to operators.

**Independent Test**: Can be tested by triggering each status condition (low battery, controller stale, no controller) and verifying all configured LEDs display the appropriate indication pattern.

**Acceptance Scenarios**:

1. **Given** any configured LED count, **When** battery voltage drops below threshold, **Then** all LEDs display the low battery warning color
2. **Given** any configured LED count, **When** controller signal becomes stale, **Then** all LEDs display the blinking controller stale pattern
3. **Given** any configured LED count, **When** no controller is connected, **Then** all LEDs display the rapid blinking no controller pattern
4. **Given** any configured LED count, **When** gradient mode is activated with a color value, **Then** all LEDs display the same gradient color
5. **Given** any configured LED count, **When** LEDs are turned off, **Then** all LEDs are extinguished

---

### User Story 3 - Memory-Efficient LED Control (Priority: P3)

The LED system allocates only the memory required for the configured number of LEDs, avoiding waste on robots with fewer LEDs while supporting robots with many LEDs.

**Why this priority**: This is an optimization that prevents memory waste but doesn't affect core functionality. On ESP32 with limited RAM, efficient memory use is valuable but not critical for basic operation.

**Independent Test**: Can be tested by examining memory usage at compile time and runtime for different LED count configurations, verifying that memory allocation scales linearly with configured LED count.

**Acceptance Scenarios**:

1. **Given** a robot configured for 1 LED, **When** the LED system initializes, **Then** memory allocation is minimal and proportional to 1 LED
2. **Given** a robot configured for 8 LEDs, **When** the LED system initializes, **Then** memory allocation is approximately 8 times that of the 1-LED configuration

---

### Edge Cases

- What happens when LED count is configured to 0? System should handle gracefully (no-op for all LED functions)
- What happens when LED count exceeds available RAM for RMT buffer? Compilation should succeed; runtime behavior depends on ESP32 memory availability
- How does system handle invalid LED count values (negative numbers)? Configuration should use unsigned integer type to prevent negative values
- What happens if the physical LED strip has fewer LEDs than configured? Only the available LEDs will light; others will be unaffected (standard NeoPixel behavior)
- What happens if the physical LED strip has more LEDs than configured? Extra LEDs remain off; only configured count will be controlled

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST allow LED count to be configured via a compile-time constant in the configuration file
- **FR-002**: System MUST support LED counts from 0 to at least 16 LEDs
- **FR-003**: System MUST allocate memory for LED color data based on configured LED count
- **FR-004**: System MUST allocate memory for LED control signals based on configured LED count
- **FR-005**: All existing LED status functions (ready, low battery, controller stale, no controller, gradient) MUST apply the same color/pattern to all configured LEDs
- **FR-006**: LED system MUST initialize RMT peripheral correctly regardless of configured LED count
- **FR-007**: LED write operation MUST transmit data for exactly the configured number of LEDs to the NeoPixel strip
- **FR-008**: System MUST handle LED count of 0 gracefully (all LED functions become no-ops)
- **FR-009**: Configuration constant MUST be located in the melty_config.h file alongside other hardware configuration values
- **FR-010**: Configuration constant MUST use a clear, descriptive name indicating it controls LED count

### Key Entities

- **LED Configuration**: Compile-time setting defining how many NeoPixel LEDs are connected to the robot
- **LED Color Data**: Runtime storage for color information for each configured LED
- **LED Control Signals**: Runtime storage for transmission data to control the NeoPixel strip
- **LED Status States**: System states that trigger visual feedback (ready, low battery, controller stale, no controller, gradient, off)

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Robot builders can change LED count by modifying a single configuration value and recompiling
- **SC-002**: System correctly controls any LED count from 1 to 16 LEDs without code changes beyond configuration
- **SC-003**: All status indicators (ready, low battery, controller warnings) are visible on all configured LEDs
- **SC-004**: Memory usage scales proportionally with configured LED count (supporting 1-16 LEDs without excessive waste or overflow)
- **SC-005**: LED visual output remains consistent and reliable for any configured LED count (no flickering, incorrect colors, or communication failures)
- **SC-006**: Existing robot control code continues to work without modification after upgrading to configurable LED system

## Assumptions

- NeoPixel LEDs are connected in series on the same data pin
- All LEDs in the strip should display the same color/pattern for all existing status functions (no per-LED control needed in this feature)
- LED count will be known at compile time and does not need runtime configuration
- Hardware peripheral configuration (communication channel, pin assignment) remains unchanged from current implementation
- LED communication protocol remains compatible with current NeoPixel strips
- Available system memory is sufficient to support configured LED count
- Maximum practical LED count is limited by available memory (assumed safe up to at least 16 LEDs)
