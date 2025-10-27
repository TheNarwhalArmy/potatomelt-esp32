# Testing Guide: Configurable NeoPixel LED Count

**Feature**: 001-configurable-neopixel  
**Date**: October 27, 2025  
**Status**: Implementation Complete - Awaiting Hardware Validation

## Overview

This document provides comprehensive testing instructions for validating the configurable NeoPixel LED feature on physical hardware. All tests require an ESP32-S3 with connected NeoPixel strip.

## Prerequisites

- ESP32-S3 development board
- NeoPixel WS2812B LED strip with configurable LED count
- Arduino IDE configured for ESP32-S3
- USB cable for programming
- 5V power supply for LED strip (if using >2 LEDs)

## Test Scenarios

### T019: Quickstart Scenarios Validation

Test each scenario from `quickstart.md` on physical hardware:

#### Scenario 1: Single LED Configuration
```cpp
#define NEOPIXEL_LED_COUNT 1
```
**Expected Results**:
- ✓ Compilation succeeds
- ✓ Boot: Single LED flashes white (100ms)
- ✓ Ready state: Single LED shows blue
- ✓ Low battery: Single LED shows red
- ✓ Controller stale: Single LED blinks red (slow)
- ✓ No controller: Single LED blinks red (fast)

#### Scenario 2: Dual LED Configuration (Default)
```cpp
#define NEOPIXEL_LED_COUNT 2
```
**Expected Results**:
- ✓ Compilation succeeds
- ✓ Boot: Both LEDs flash white (100ms)
- ✓ Ready state: Both LEDs show blue
- ✓ Low battery: Both LEDs show red
- ✓ Controller stale: Both LEDs blink red (slow)
- ✓ No controller: Both LEDs blink red (fast)

#### Scenario 3: Quad LED Configuration
```cpp
#define NEOPIXEL_LED_COUNT 4
```
**Expected Results**:
- ✓ Compilation succeeds
- ✓ Boot: All 4 LEDs flash white (100ms)
- ✓ Ready state: All 4 LEDs show blue
- ✓ Low battery: All 4 LEDs show red
- ✓ All status states work across all 4 LEDs

#### Scenario 4: Octal LED Configuration
```cpp
#define NEOPIXEL_LED_COUNT 8
```
**Expected Results**:
- ✓ Compilation succeeds
- ✓ Boot: All 8 LEDs flash white (100ms)
- ✓ Ready state: All 8 LEDs show blue
- ✓ All status states synchronized across all 8 LEDs

---

### T020: Edge Case Validation

#### Edge Case 1: Zero LEDs (Graceful Degradation)
```cpp
#define NEOPIXEL_LED_COUNT 0
```
**Expected Results**:
- ✓ Compilation succeeds with no warnings
- ✓ Boot: No LED activity (no flash)
- ✓ All LED function calls are no-ops
- ✓ No runtime errors or crashes
- ✓ Robot operates normally without LEDs

**Validation Method**:
1. Set `NEOPIXEL_LED_COUNT` to 0
2. Compile and upload to ESP32-S3
3. Verify no LED activity occurs
4. Verify robot functions normally
5. Check serial output for any errors

---

#### Edge Case 2: Maximum LEDs (16)
```cpp
#define NEOPIXEL_LED_COUNT 16
```
**Expected Results**:
- ✓ Compilation succeeds
- ✓ Boot: All 16 LEDs flash white (100ms)
- ✓ Ready state: All 16 LEDs show blue
- ✓ All status states synchronized across all 16 LEDs
- ✓ Memory usage within acceptable limits

**Validation Method**:
1. Set `NEOPIXEL_LED_COUNT` to 16
2. Compile and note memory usage from build output
3. Upload to ESP32-S3 with 16-LED strip
4. Verify all 16 LEDs illuminate
5. Test all status states
6. Confirm no memory-related crashes

**Expected Memory Footprint**:
- `pixel_color[]`: 48 bytes (16 × 3)
- `led_data[]`: 1536 bytes (16 × 3 × 8 × 4)
- Total: ~1584 bytes (0.31% of 512KB SRAM)

---

#### Edge Case 3: Exceeding Maximum (17 LEDs - Should FAIL)
```cpp
#define NEOPIXEL_LED_COUNT 17
```
**Expected Results**:
- ✗ Compilation FAILS with clear error message
- ✓ Error message: "NEOPIXEL_LED_COUNT must be between 0 and 16"
- ✓ Failure occurs at compile time (not runtime)

**Validation Method**:
1. Set `NEOPIXEL_LED_COUNT` to 17
2. Attempt to compile
3. Verify compilation fails
4. Verify error message is clear and actionable
5. Confirm no binary is generated

---

#### Edge Case 4: Mismatched Physical LED Count

**Scenario 4a: Configured Count > Physical Count**
```cpp
#define NEOPIXEL_LED_COUNT 8  // But strip only has 4 LEDs
```
**Expected Results**:
- ✓ Compilation succeeds
- ✓ Only available LEDs (4) illuminate
- ✓ No errors or crashes
- ✓ Configured but missing LEDs simply remain off

---

**Scenario 4b: Configured Count < Physical Count**
```cpp
#define NEOPIXEL_LED_COUNT 2  // But strip has 8 LEDs
```
**Expected Results**:
- ✓ Compilation succeeds
- ✓ Only first 2 LEDs illuminate
- ✓ Remaining 6 LEDs stay dark
- ✓ No impact on robot operation

---

#### Edge Case 5: Custom Color Configuration
```cpp
#define LED_COLOR_READY_R 0
#define LED_COLOR_READY_G 255
#define LED_COLOR_READY_B 0  // Green instead of blue
```
**Expected Results**:
- ✓ Compilation succeeds
- ✓ Ready state shows green instead of blue
- ✓ All other states use default colors
- ✓ Color change affects all configured LEDs

---

## Memory Footprint Analysis (T015 - User Story 3)

### Test Method
1. Compile with different LED counts: 1, 2, 4, 8, 16
2. Record memory usage from Arduino IDE build output
3. Verify linear scaling

### Expected Results

| LED Count | pixel_color | led_data | Total    | % of SRAM |
|-----------|-------------|----------|----------|-----------|
| 1         | 3 bytes     | 96 bytes | 99 bytes | 0.02%     |
| 2         | 6 bytes     | 192 bytes| 198 bytes| 0.04%     |
| 4         | 12 bytes    | 384 bytes| 396 bytes| 0.08%     |
| 8         | 24 bytes    | 768 bytes| 792 bytes| 0.15%     |
| 16        | 48 bytes    | 1536 bytes|1584 bytes| 0.31%     |

**Validation Criteria**:
- ✓ Memory usage scales linearly with LED count
- ✓ Even at 16 LEDs, usage is negligible (<1% of SRAM)
- ✓ No unexpected memory overhead

---

## Status Indicator Tests

For each LED count configuration (1, 2, 4, 8), verify all status indicators:

### Test Matrix

| Robot State | Expected Behavior | Color |
|-------------|------------------|-------|
| **Boot** | Brief white flash (100ms on, 50ms off) | White (255, 255, 255) |
| **No Controller** | Rapid blink (200ms on, 800ms off) | Red (configurable) |
| **Controller Stale** | Slow blink (800ms on, 200ms off) | Red (configurable) |
| **Ready** | Solid | Blue (configurable) |
| **Low Battery** | Solid | Red (configurable) |
| **Gradient (0)** | Solid | Red (255, 0, 0) |
| **Gradient (50)** | Solid | Yellow (255, 255, 0) |
| **Gradient (100)** | Solid | Green (0, 255, 0) |
| **Off** | All LEDs extinguished | Black (0, 0, 0) |

---

## Performance Tests

### LED Update Timing
**Requirement**: LED update must complete within 30µs per LED

**Test Method**:
1. Add timing instrumentation to `write_pixel()`:
   ```cpp
   unsigned long start = micros();
   rmt_write_items(NEOPIXEL_RMT, led_data, NEOPIXEL_LED_COUNT * 3 * 8, false);
   unsigned long elapsed = micros() - start;
   Serial.printf("LED update: %lu us\n", elapsed);
   ```
2. Test with maximum LED count (16)
3. Verify total time < 480µs (30µs × 16)

**Expected Results**:
- ✓ 1 LED: ~30µs
- ✓ 8 LEDs: ~240µs
- ✓ 16 LEDs: ~480µs
- ✓ All values within acceptable limits

---

## Integration Tests

### Startup Sequence Test
1. Power on robot with configured LED count
2. Observe startup diagnostic
3. Verify timing: 100ms flash + 50ms off = 150ms total

### Battery Warning Integration
1. Drain battery below threshold
2. Verify low battery color displays on all LEDs
3. Confirm all LEDs show same color simultaneously

### Controller Integration
1. Power on without controller connection
2. Verify "no controller" blink pattern
3. Connect controller
4. Verify transition to ready state
5. Disconnect controller mid-operation
6. Verify "controller stale" blink pattern

---

## Test Results Log

### Test Run: [DATE]
**Tester**: [NAME]  
**Hardware**: ESP32-S3, [X] LED NeoPixel strip  
**Firmware Version**: [COMMIT HASH]

| Test ID | Configuration | Result | Notes |
|---------|--------------|--------|-------|
| US1-1 | 1 LED | ☐ PASS ☐ FAIL | |
| US1-2 | 2 LEDs | ☐ PASS ☐ FAIL | |
| US1-4 | 4 LEDs | ☐ PASS ☐ FAIL | |
| US1-8 | 8 LEDs | ☐ PASS ☐ FAIL | |
| US2-Ready | Ready color | ☐ PASS ☐ FAIL | |
| US2-LowBat | Low battery color | ☐ PASS ☐ FAIL | |
| US2-CtrlStale | Controller stale | ☐ PASS ☐ FAIL | |
| US2-NoCtrl | No controller | ☐ PASS ☐ FAIL | |
| US3-Memory | Memory scaling | ☐ PASS ☐ FAIL | |
| US4-Startup | Startup diagnostic | ☐ PASS ☐ FAIL | |
| Edge-0 | 0 LEDs | ☐ PASS ☐ FAIL | |
| Edge-16 | 16 LEDs | ☐ PASS ☐ FAIL | |
| Edge-17 | 17 LEDs (fail) | ☐ PASS ☐ FAIL | |

---

## Troubleshooting Common Issues

### Issue: LEDs don't light up
**Check**:
- Physical wiring (data pin, power, ground)
- `NEOPIXEL_PIN` matches hardware configuration
- 5V power supply connected for strips >2 LEDs

### Issue: Wrong number of LEDs light up
**Check**:
- `NEOPIXEL_LED_COUNT` matches physical LED count
- Physical strip has enough LEDs

### Issue: Colors are wrong
**Check**:
- Some NeoPixel variants use different color order (RGB vs GRB)
- Verify WS2812B compatibility
- Check color configuration constants

### Issue: Compilation fails
**Check**:
- `NEOPIXEL_LED_COUNT` is between 0 and 16
- Configuration constants are defined in `melty_config.h`

---

## Sign-Off Criteria

All tests must pass before merging feature branch:

- [ ] All quickstart scenarios validated (1, 2, 4, 8 LEDs)
- [ ] All edge cases tested (0, 16, 17 LEDs)
- [ ] Memory footprint verified (linear scaling)
- [ ] All status indicators working correctly
- [ ] Startup diagnostic confirmed
- [ ] Performance requirements met (<30µs per LED)
- [ ] Integration with controller system verified
- [ ] Custom color configuration tested

**Feature is ready for production when all checkboxes are marked.**
