---

description: "Task list for Configurable NeoPixel LED Count feature"
---

# Tasks: Configurable NeoPixel LED Count

**Input**: Design documents from `/specs/001-configurable-neopixel/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, quickstart.md

**Tests**: This feature does NOT include automated test tasks per constitution VIII (hardware-in-the-loop testing on physical robot required).

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3, US4)
- Include exact file paths in descriptions

## Path Conventions

- **Embedded single project**: `potatomelt/src/` at repository root
- Configuration in `potatomelt/src/melty_config.h`
- LED subsystem in `potatomelt/src/subsystems/led.h` and `potatomelt/src/subsystems/led.cpp`

---

## Phase 1: Setup (Configuration Infrastructure)

**Purpose**: Add configuration constants to centralize LED count and color settings

- [ ] T001 Add LED count configuration constant to potatomelt/src/melty_config.h
- [ ] T002 Add ready state color configuration constants to potatomelt/src/melty_config.h
- [ ] T003 Add low battery color configuration constants to potatomelt/src/melty_config.h
- [ ] T004 Add controller warning color configuration constants to potatomelt/src/melty_config.h

---

## Phase 2: Foundational (Core LED System Updates)

**Purpose**: Core LED array sizing and validation that MUST be complete before user stories

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [ ] T005 Replace hardcoded pixel_color array size with NEOPIXEL_LED_COUNT-based sizing in potatomelt/src/subsystems/led.cpp
- [ ] T006 Replace hardcoded led_data array size with NEOPIXEL_LED_COUNT-based sizing in potatomelt/src/subsystems/led.cpp
- [ ] T007 Add compile-time validation (static_assert) for LED count range (0-16) in potatomelt/src/subsystems/led.cpp

**Checkpoint**: Foundation ready - LED arrays are dynamically sized and validated

---

## Phase 3: User Story 1 - Configure LED Count at Compile Time (Priority: P1) 🎯 MVP

**Goal**: Robot builders can configure the number of NeoPixel LEDs by changing a single configuration value, supporting robots with 1-16 LEDs

**Independent Test**: Change NEOPIXEL_LED_COUNT configuration value (1, 2, 4, 8), compile, and verify that status indicators correctly illuminate all configured LEDs on physical hardware

### Implementation for User Story 1

- [ ] T008 [US1] Update leds_on_rgb() to set color for all NEOPIXEL_LED_COUNT LEDs in potatomelt/src/subsystems/led.cpp
- [ ] T009 [US1] Update leds_off() to clear all NEOPIXEL_LED_COUNT LEDs in potatomelt/src/subsystems/led.cpp
- [ ] T010 [US1] Update write_pixel() to transmit data for NEOPIXEL_LED_COUNT LEDs in potatomelt/src/subsystems/led.cpp

**Checkpoint**: At this point, LED count is fully configurable. Test with different counts (1, 2, 4, 8) on physical hardware

---

## Phase 4: User Story 2 - Dynamic Status Display Across All LEDs (Priority: P2)

**Goal**: Robot operators can see consistent status information across all configured LEDs, with all LEDs displaying the same color/pattern for system-wide states

**Independent Test**: Trigger each status condition (low battery, controller stale, no controller) and verify all configured LEDs display the appropriate indication pattern

### Implementation for User Story 2

- [ ] T011 [US2] Replace hardcoded ready color with configuration constants in leds_on_ready() in potatomelt/src/subsystems/led.cpp
- [ ] T012 [US2] Replace hardcoded low battery color with configuration constants in leds_on_low_battery() in potatomelt/src/subsystems/led.cpp
- [ ] T013 [US2] Replace hardcoded controller warning color with configuration constants in leds_on_controller_stale() in potatomelt/src/subsystems/led.cpp
- [ ] T014 [US2] Replace hardcoded controller warning color with configuration constants in leds_on_no_controller() in potatomelt/src/subsystems/led.cpp

**Checkpoint**: At this point, all status indicators use configurable colors. Test each status state on physical hardware

---

## Phase 5: User Story 3 - Memory-Efficient LED Control (Priority: P3)

**Goal**: LED system allocates only the memory required for the configured number of LEDs

**Independent Test**: Examine memory usage at compile time and runtime for different LED count configurations (1, 2, 8, 16), verify memory allocation scales linearly

### Implementation for User Story 3

- [ ] T015 [US3] Verify memory footprint scales correctly with LED count using ESP32 build output analysis

**Checkpoint**: Memory optimization is inherent in the static array sizing from Phase 2. Verification confirms proper scaling

---

## Phase 6: User Story 4 - Startup LED Diagnostic (Priority: P3)

**Goal**: Robot builders can visually verify the configured LED count through a brief startup sequence

**Independent Test**: Observe LED behavior during system initialization with different LED count configurations (1, 2, 4, 8)

### Implementation for User Story 4

- [ ] T016 [US4] Implement startup diagnostic sequence in LED constructor in potatomelt/src/subsystems/led.cpp

**Checkpoint**: All user stories are now complete. Startup flash confirms LED configuration visually

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Documentation and final validation

- [ ] T017 [P] Update inline code documentation for new configuration constants in potatomelt/src/melty_config.h
- [ ] T018 [P] Add comments explaining LED count limits and validation in potatomelt/src/subsystems/led.cpp
- [ ] T019 Validate configuration with quickstart.md scenarios on physical hardware (1, 2, 4, 8 LEDs)
- [ ] T020 Test edge cases: 0 LEDs (graceful no-op), 16 LEDs (maximum), 17 LEDs (compilation error)

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3-6)**: All depend on Foundational phase completion
  - User Story 1 (P1): Can start after Foundational - No dependencies on other stories
  - User Story 2 (P2): Depends on User Story 1 (needs working leds_on_rgb function)
  - User Story 3 (P3): No implementation needed, verification only after Phase 2
  - User Story 4 (P3): Can start after Foundational - No dependencies on other stories
- **Polish (Phase 7)**: Depends on all user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Requires Foundational (Phase 2) - core capability
- **User Story 2 (P2)**: Requires User Story 1 - builds on working LED functions
- **User Story 3 (P3)**: Requires Foundational (Phase 2) - verification task only
- **User Story 4 (P3)**: Requires Foundational (Phase 2) - independent diagnostic feature

### Within Each User Story

- User Story 1: leds_on_rgb → leds_off → write_pixel (sequential on same file)
- User Story 2: All four status function updates can proceed sequentially (same file edits)
- User Story 3: Verification only, no implementation dependencies
- User Story 4: Single constructor modification, no dependencies

### Parallel Opportunities

- **Phase 1 (Setup)**: All four configuration constant additions (T001-T004) can be done in parallel (same file but different sections)
- **Phase 2 (Foundational)**: Array sizing tasks (T005-T006) can be done in parallel (different array declarations), validation (T007) can follow
- **User Story 2**: All four status color updates (T011-T014) could be done in parallel (different functions in same file, but sequential is safer)
- **Phase 7 (Polish)**: Documentation tasks (T017-T018) can be done in parallel (different files)

**Note**: Since this is an embedded single-file modification project with most changes in led.cpp, true parallelization is limited. Sequential execution within each phase is recommended to avoid merge conflicts.

---

## Parallel Example: Phase 1 Setup

```bash
# All configuration constants can be added together (different sections of melty_config.h):
Task: "Add LED count configuration constant to potatomelt/src/melty_config.h"
Task: "Add ready state color configuration constants to potatomelt/src/melty_config.h"
Task: "Add low battery color configuration constants to potatomelt/src/melty_config.h"
Task: "Add controller warning color configuration constants to potatomelt/src/melty_config.h"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup (add NEOPIXEL_LED_COUNT constant)
2. Complete Phase 2: Foundational (resize arrays, add validation)
3. Complete Phase 3: User Story 1 (update LED functions for configurable count)
4. **STOP and VALIDATE**: Test with 1, 2, 4, 8 LEDs on physical hardware
5. Deploy if ready - basic configurable LED support is functional

### Incremental Delivery

1. Complete Setup + Foundational → Arrays sized correctly, validation in place
2. Add User Story 1 → Test with different LED counts → Deploy (MVP!)
3. Add User Story 2 → Test status colors → Deploy (configurable colors added)
4. Add User Story 3 → Verify memory scaling → Validate (optimization confirmed)
5. Add User Story 4 → Test startup diagnostic → Deploy (diagnostic added)
6. Complete Polish → Full documentation and edge case validation

### Sequential Execution (Recommended for Single Developer)

1. Complete all of Phase 1 (T001-T004) - add all configuration constants
2. Complete all of Phase 2 (T005-T007) - resize arrays and add validation
3. Complete User Story 1 (T008-T010) - make LED count configurable
4. Hardware test with different LED counts
5. Complete User Story 2 (T011-T014) - make colors configurable
6. Hardware test status indicators
7. Complete User Story 3 (T015) - verify memory scaling
8. Complete User Story 4 (T016) - add startup diagnostic
9. Hardware test startup sequence
10. Complete Polish (T017-T020) - documentation and final validation

---

## Hardware Validation Checklist

After completing each user story, validate on physical ESP32-S3 with NeoPixel strip:

### User Story 1 Validation
- [ ] Configure 1 LED, compile, upload, verify single LED lights in ready state
- [ ] Configure 2 LEDs, compile, upload, verify both LEDs light in ready state
- [ ] Configure 4 LEDs, compile, upload, verify all 4 LEDs light in ready state
- [ ] Configure 8 LEDs, compile, upload, verify all 8 LEDs light in ready state

### User Story 2 Validation
- [ ] Trigger low battery condition, verify all LEDs show low battery color
- [ ] Trigger controller stale condition, verify all LEDs show blinking controller stale pattern
- [ ] Trigger no controller condition, verify all LEDs show rapid blinking pattern
- [ ] Test gradient mode, verify all LEDs show gradient color
- [ ] Test LEDs off, verify all LEDs are extinguished

### User Story 3 Validation
- [ ] Build with 1 LED, note memory usage from build output
- [ ] Build with 8 LEDs, verify memory usage is ~8x that of 1 LED
- [ ] Build with 16 LEDs, verify memory usage is ~16x that of 1 LED

### User Story 4 Validation
- [ ] Power on robot with 1 LED, observe startup flash
- [ ] Power on robot with 4 LEDs, observe startup flash on all LEDs
- [ ] Power on robot with 8 LEDs, observe startup flash on all LEDs

### Edge Case Validation
- [ ] Configure 0 LEDs, verify compilation succeeds and all LED functions are no-ops
- [ ] Configure 16 LEDs, verify compilation succeeds and all 16 LEDs work
- [ ] Configure 17 LEDs, verify compilation fails with clear error message

---

## Summary

**Total Tasks**: 20
**MVP Tasks (User Story 1)**: Setup (4) + Foundational (3) + US1 (3) = 10 tasks
**Task Distribution by User Story**:
- Setup: 4 tasks (configuration constants)
- Foundational: 3 tasks (array sizing and validation)
- User Story 1: 3 tasks (configurable LED count)
- User Story 2: 4 tasks (configurable status colors)
- User Story 3: 1 task (memory verification)
- User Story 4: 1 task (startup diagnostic)
- Polish: 4 tasks (documentation and validation)

**Parallel Opportunities**: Limited due to single-file modifications (led.cpp)
- Best parallelization: Phase 1 configuration constants
- Most tasks are sequential edits to same file for safety

**Independent Test Criteria**:
- US1: Change LED count, verify all configured LEDs work
- US2: Trigger status conditions, verify color display
- US3: Compare memory usage across different LED counts
- US4: Observe startup sequence with different counts

**Suggested MVP Scope**: 
- Phases 1-3 (Setup + Foundational + User Story 1)
- Delivers core configurable LED count functionality
- 10 tasks total for working MVP
- Can deploy and validate before adding color customization

**Format Validation**: ✅ All tasks follow checklist format (checkbox, ID, labels where appropriate, file paths)
