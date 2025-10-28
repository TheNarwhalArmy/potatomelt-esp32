# Implementation Plan: Configurable NeoPixel LED Count

**Branch**: `001-configurable-neopixel` | **Date**: October 27, 2025 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/001-configurable-neopixel/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Enable the LED subsystem to support configurable NeoPixel LED counts (0-16 LEDs) by replacing hardcoded 2-LED arrays with dynamically-sized arrays based on a compile-time constant. All status functions (ready, low battery, controller warnings, gradient) will apply the same color to all configured LEDs. Memory allocation will scale with LED count, and status indicator colors will be configurable via compile-time constants. A brief startup diagnostic sequence will visually confirm LED configuration.

## Technical Context

**Language/Version**: C++ (Arduino framework), ESP32-S3 (dual-core Xtensa LX7)  
**Primary Dependencies**: Arduino ESP32, ESP32 RMT peripheral, NeoPixel WS2812B LEDs  
**Storage**: N/A (compile-time configuration only)  
**Testing**: Hardware-in-the-loop on physical robot (per constitution VIII)  
**Target Platform**: ESP32-S3 embedded platform with FreeRTOS
**Project Type**: Single embedded project  
**Performance Goals**: LED update MUST complete within 30µs (per constitution Performance Standards)  
**Constraints**: 
- No dynamic allocation in time-critical paths (constitution V)
- Memory-efficient scaling for 0-16 LEDs
- RMT peripheral limited to 64 rmt_item32_t elements per channel (hardware constraint)
**Scale/Scope**: Support 0-16 NeoPixel LEDs with proportional memory allocation

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

✅ **I. Performance-First Architecture**: Changes are additive to existing LED system. No additional function call overhead in critical loops. Uses compile-time constants and static array sizing.

✅ **II. Hardware Integration Discipline**: New configuration constants centralized in `melty_config.h` per existing pattern. RMT peripheral configuration unchanged except for dynamic buffer sizing.

✅ **III. Real-Time Safety Critical**: No impact on safety systems. LED system is status indication only, not safety-critical. No changes to failsafe or control logic.

✅ **IV. Documentation Excellence**: All new configuration constants will have inline comments with units and purpose. Code changes will document scaling behavior.

✅ **V. Embedded Systems Best Practices**: No changes to task structure, core allocation, or inter-task communication. Static memory allocation scaled by compile-time constant.

✅ **VI. Modularity & Extensibility**: Changes isolated to LED subsystem. Configuration centralized. Existing API preserved. Zero impact on other subsystems.

✅ **VII. Combat Robot Domain Standards**: No impact on motor control, PID, heading, or telemetry systems.

✅ **VIII. Testing Philosophy**: Will validate on physical hardware with different LED counts (1, 2, 4, 8 LEDs). No automated tests required.

✅ **IX. Persistent Configuration**: LED count is compile-time only, no NVS storage needed.

✅ **X. Code Organization**: Changes confined to `src/subsystems/led.h`, `src/subsystems/led.cpp`, and `melty_config.h` per existing patterns.

**Result**: All constitution principles satisfied. No violations requiring justification.

## Project Structure

### Documentation (this feature)

```text
specs/001-configurable-neopixel/
├── plan.md              # This file
├── research.md          # Phase 0 output
├── data-model.md        # Phase 1 output
├── quickstart.md        # Phase 1 output
└── checklists/
    └── requirements.md  # Pre-existing checklist
```

### Source Code (repository root)

```text
potatomelt/
├── potatomelt.ino          # Main Arduino sketch (no changes)
└── src/
    ├── melty_config.h      # ADD: LED count + color configuration constants
    ├── subsystems/
    │   ├── led.h           # MODIFY: Dynamic array sizing
    │   └── led.cpp         # MODIFY: Configurable LED count logic
    └── (other files unchanged)
```

**Structure Decision**: Using existing single-project embedded structure. Changes isolated to LED subsystem and configuration header per constitution X.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

N/A - No constitution violations.

---

## Phase 0: Research - COMPLETE ✓

See [research.md](./research.md) for full details.

**Key Decisions**:
- Support up to 16 LEDs using compile-time array sizing
- Use `#define` constants for configuration (consistent with existing style)
- Implement static_assert for compile-time validation
- Add brief startup flash (150ms) for visual confirmation
- Maintain existing RMT timing (WS2812B-compatible)

---

## Phase 1: Design & Implementation - COMPLETE ✓

### Generated Artifacts

1. **[data-model.md](./data-model.md)**: Complete data structure documentation
   - Configuration entity definitions
   - Runtime array specifications
   - Memory footprint analysis
   - API contract preservation

2. **[quickstart.md](./quickstart.md)**: User-facing configuration guide
   - Step-by-step LED count configuration
   - Color customization instructions
   - Hardware wiring reference
   - Troubleshooting guide

3. **Implementation Plan**: Ready for execution
   - Files to modify: `melty_config.h`, `led.cpp`
   - Specific changes documented in data-model.md
   - No changes to `led.h` (API preserved)

### Constitution Re-Check (Post-Design)

Re-evaluating all principles after design phase:

✅ **I. Performance-First**: Design uses compile-time constants, zero runtime overhead, static arrays  
✅ **II. Hardware Integration**: Configuration will be centralized in `melty_config.h`  
✅ **III. Safety Critical**: No impact on safety systems  
✅ **IV. Documentation**: Design includes full documentation requirements  
✅ **V. Embedded Best Practices**: No dynamic allocation, proper static sizing  
✅ **VI. Modularity**: Changes isolated to LED subsystem  
✅ **VII. Domain Standards**: No impact on combat robot control systems  
✅ **VIII. Testing**: Hardware validation approach documented  
✅ **IX. Persistent Config**: Compile-time only, appropriate for this feature  
✅ **X. Code Organization**: Follows established patterns

**Final Result**: All constitution principles satisfied in design phase.

---

## Post-Implementation Notes

### Architecture Lessons Learned

**Issue**: Watchdog Timer Boot Loop (Fixed in commit d156e1d)

During initial implementation, the LED startup diagnostic was placed in the `LED()` constructor with `delay()` calls. This caused an ESP32 watchdog timer boot loop because:
- Global object constructors run before ESP32 system initialization
- Delays in constructors block the boot sequence
- ESP32 watchdog timer triggers on blocked boot (~300ms timeout)

**Solution**: Separated construction from initialization by:
1. Creating explicit `LED::init()` method
2. Moving hardware setup and delays to `init()`
3. Calling `leds.init()` from `Robot::init()` after system startup
4. Keeping constructor minimal (no blocking operations)

**Constitution Alignment**: This fix reinforces **Constitution V (Embedded Systems Best Practices)**:
> "Constructors must not block or delay. All hardware initialization requiring delays must be in explicit init() methods called after system startup."

This pattern should be applied to all subsystems requiring hardware initialization.

## Next Steps (Not Part of /speckit.plan)

The `/speckit.plan` command completes after Phase 1 design. To continue:

1. **Create Tasks**: Run `/speckit.tasks` to generate Phase 2 task breakdown
2. **Implementation**: Execute tasks to implement the planned changes
3. **Hardware Validation**: Test on physical robot with different LED counts
4. **Merge**: Once validated, merge feature branch to main

---

## Implementation Report

**Branch**: `001-configurable-neopixel`  
**Plan Path**: `c:\Documents\potatomelt-esp32\specs\001-configurable-neopixel\plan.md`  
**Generated Artifacts**:
- ✅ `research.md` - Technical research and decisions
- ✅ `data-model.md` - Data structures and entities  
- ✅ `quickstart.md` - User configuration guide
- ✅ Agent context updated (`.github/copilot-instructions.md`)

**Status**: Phase 0 (Research) and Phase 1 (Design) complete. Ready for Phase 2 (Tasks) and implementation.

