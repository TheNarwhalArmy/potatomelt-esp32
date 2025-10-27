# Data Model: Configurable NeoPixel LED Count

**Feature**: 001-configurable-neopixel  
**Date**: October 27, 2025  
**Status**: Complete

## Overview

This document defines the data structures and configuration entities for the configurable LED system. All entities are compile-time constants or static arrays - no runtime data structures are introduced.

## Configuration Entities

### 1. LED Count Configuration

**Entity**: `NEOPIXEL_LED_COUNT`  
**Type**: Compile-time constant (`#define`)  
**Location**: `melty_config.h`  
**Valid Range**: 0 to 16 (inclusive)

**Purpose**: Defines the number of NeoPixel LEDs connected to the robot's data line.

**Validation Rules**:
- MUST be a non-negative integer
- MUST be ≤ 16 (enforced by static_assert)
- Value of 0 is valid (all LED functions become no-ops)

**Default Value**: 2 (maintains backward compatibility with existing robots)

**Example**:
```cpp
#define NEOPIXEL_LED_COUNT 4  // Robot has 4 NeoPixel LEDs
```

---

### 2. Ready State Color Configuration

**Entities**: 
- `LED_COLOR_READY_R` (Red component)
- `LED_COLOR_READY_G` (Green component)
- `LED_COLOR_READY_B` (Blue component)

**Type**: Compile-time constants (`#define`)  
**Location**: `melty_config.h`  
**Valid Range**: 0 to 255 (8-bit RGB values)

**Purpose**: Defines the RGB color displayed when robot is in ready state (armed and operational).

**Default Values**:
```cpp
#define LED_COLOR_READY_R 0
#define LED_COLOR_READY_G 0
#define LED_COLOR_READY_B 255  // Blue
```

---

### 3. Low Battery Warning Color Configuration

**Entities**: 
- `LED_COLOR_LOW_BATTERY_R`
- `LED_COLOR_LOW_BATTERY_G`
- `LED_COLOR_LOW_BATTERY_B`

**Type**: Compile-time constants (`#define`)  
**Location**: `melty_config.h`  
**Valid Range**: 0 to 255

**Purpose**: Defines the RGB color displayed when battery voltage is below safe threshold.

**Default Values**:
```cpp
#define LED_COLOR_LOW_BATTERY_R 255  // Red
#define LED_COLOR_LOW_BATTERY_G 0
#define LED_COLOR_LOW_BATTERY_B 0
```

---

### 4. Controller Warning Color Configuration

**Entities**: 
- `LED_COLOR_CONTROLLER_WARNING_R`
- `LED_COLOR_CONTROLLER_WARNING_G`
- `LED_COLOR_CONTROLLER_WARNING_B`

**Type**: Compile-time constants (`#define`)  
**Location**: `melty_config.h`  
**Valid Range**: 0 to 255

**Purpose**: Defines the RGB color displayed when controller is stale or disconnected (used with blinking patterns).

**Default Values**:
```cpp
#define LED_COLOR_CONTROLLER_WARNING_R 255  // Red
#define LED_COLOR_CONTROLLER_WARNING_G 0
#define LED_COLOR_CONTROLLER_WARNING_B 0
```

---

## Runtime Data Structures

### 5. LED Color Buffer

**Entity**: `pixel_color[]`  
**Type**: Static array of `uint8_t`  
**Size**: `NEOPIXEL_LED_COUNT * 3` bytes  
**Location**: `led.cpp` (file-scope static)

**Purpose**: Stores the current RGB color data for all configured LEDs in GRB format (Green, Red, Blue order required by WS2812B protocol).

**Structure**:
```cpp
uint8_t pixel_color[NEOPIXEL_LED_COUNT * 3];
// Layout for 2 LEDs:
// [0] = LED0_Green
// [1] = LED0_Red
// [2] = LED0_Blue
// [3] = LED1_Green
// [4] = LED1_Red
// [5] = LED1_Blue
```

**Relationships**:
- Sized by `NEOPIXEL_LED_COUNT` configuration constant
- Written by `leds_on_rgb()` and `leds_off()`
- Read by `write_pixel()` to generate RMT timing data

**Validation Rules**:
- Each byte must be 0-255 (enforced by uint8_t type)
- Array size must accommodate all configured LEDs

---

### 6. RMT Timing Buffer

**Entity**: `led_data[]`  
**Type**: Static array of `rmt_item32_t`  
**Size**: `NEOPIXEL_LED_COUNT * 3 * 8` elements  
**Location**: `led.cpp` (file-scope static)

**Purpose**: Stores RMT peripheral timing data that encodes LED color information as WS2812B-compatible pulse widths.

**Structure**:
- Each LED color byte (8 bits) requires 8 rmt_item32_t elements
- Each LED (3 bytes GRB) requires 24 rmt_item32_t elements
- Total elements = `NEOPIXEL_LED_COUNT × 3 bytes × 8 bits`

**Relationships**:
- Sized by `NEOPIXEL_LED_COUNT` configuration constant
- Generated from `pixel_color[]` by `write_pixel()`
- Transmitted to NeoPixel strip via `rmt_write_items()`

**Validation Rules**:
- Total size must not exceed RMT peripheral capabilities
- Static assertion enforces maximum LED count

---

## State Transitions

### LED Status State Machine

The LED system displays different colors/patterns based on robot state:

```
┌─────────────────┐
│   SYSTEM BOOT   │
│  (White flash)  │
└────────┬────────┘
         │
         v
┌─────────────────┐
│  NO CONTROLLER  │──────┐
│ (Rapid blink)   │      │ Controller connects
└────────┬────────┘      │
         │               v
         │        ┌──────────────┐
         │        │ READY STATE  │
         │        │   (Blue)     │
         │        └──────┬───────┘
         │               │
         │               ├─> Low battery ──> LOW BATTERY (Red)
         │               │
         │               ├─> Controller stale ──> CONTROLLER STALE (Slow blink)
         │               │
         │               └─> Spinning ──> GRADIENT (Red-Yellow-Green)
         │
         └─> LED count = 0 ──> All functions are no-ops

```

**State Definitions**:

1. **System Boot** (transient): White flash for 100ms, then off for 50ms
2. **No Controller**: Red rapid blink (200ms on, 800ms off cycle)
3. **Controller Stale**: Red slow blink (800ms on, 200ms off cycle)
4. **Ready**: Solid blue (or configured ready color)
5. **Low Battery**: Solid red (or configured low battery color)
6. **Gradient**: Color calculated based on input value (0-100)
7. **Off**: All LEDs extinguished

---

## Memory Footprint Analysis

| LED Count | pixel_color[] | led_data[] | Total Memory | Percentage of 512KB SRAM |
|-----------|---------------|------------|--------------|--------------------------|
| 0         | 0 bytes       | 0 bytes    | 0 bytes      | 0%                       |
| 1         | 3 bytes       | 96 bytes   | 99 bytes     | 0.02%                    |
| 2         | 6 bytes       | 192 bytes  | 198 bytes    | 0.04%                    |
| 4         | 12 bytes      | 384 bytes  | 396 bytes    | 0.08%                    |
| 8         | 24 bytes      | 768 bytes  | 792 bytes    | 0.15%                    |
| 16        | 48 bytes      | 1536 bytes | 1584 bytes   | 0.31%                    |

**Calculation**:
- `pixel_color[]` = `NEOPIXEL_LED_COUNT × 3` bytes
- `led_data[]` = `NEOPIXEL_LED_COUNT × 3 × 8 × 4` bytes (4 bytes per rmt_item32_t)
- Total = pixel_color + led_data

**Observation**: Even at maximum 16 LEDs, memory usage is negligible (0.31% of available SRAM).

---

## API Contracts

### Public Methods (unchanged from current implementation)

All public methods maintain existing signatures for backward compatibility:

```cpp
class LED {
public:
    LED();  // Constructor: Initialize RMT, perform startup diagnostic
    
    void leds_on_ready();                    // Display ready state color
    void leds_on_low_battery();              // Display low battery color
    void leds_on_controller_stale();         // Display blinking controller stale pattern
    void leds_on_no_controller();            // Display rapid blinking no controller pattern
    void leds_on_gradient(int color);        // Display calculated gradient color (0-100)
    void leds_off();                         // Turn off all LEDs
};
```

**Behavioral Changes**:
- All methods now affect `NEOPIXEL_LED_COUNT` LEDs instead of hardcoded 2 LEDs
- Color values come from configuration constants instead of hardcoded literals
- Constructor performs brief startup diagnostic (150ms delay)

**Compatibility**:
- Existing caller code requires no modifications
- Default configuration (2 LEDs, original colors) preserves exact original behavior
- Zero LED count causes all methods to become no-ops (graceful degradation)

---

## Validation Summary

| Requirement | Validation Method | Enforcement Point |
|-------------|------------------|-------------------|
| FR-001: Configurable LED count | Compile-time constant | `melty_config.h` |
| FR-002: Support 0-16 LEDs | static_assert | `led.cpp` |
| FR-002a: Compile error if count > 16 | static_assert | `led.cpp` |
| FR-003: Memory scaled by count | Array sizing | `led.cpp` |
| FR-011: Configurable colors | Compile-time constants | `melty_config.h` |
| FR-013: Startup diagnostic | Constructor implementation | `led.cpp` |

---

## Summary

The data model consists entirely of compile-time configuration constants and statically-sized arrays. No runtime dynamic structures are introduced. Memory usage scales linearly with configured LED count and remains negligible even at the maximum supported count. All configuration is centralized in `melty_config.h` per existing project conventions.
