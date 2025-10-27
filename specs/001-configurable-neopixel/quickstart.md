 # Quick Start: Configurable NeoPixel LED Count

**Feature**: 001-configurable-neopixel  
**Date**: October 27, 2025

## Overview

This guide explains how to configure your robot to use a custom number of NeoPixel LEDs and customize the status indicator colors.

## Prerequisites

- PotatoMelt-ESP32 codebase checked out on branch `001-configurable-neopixel`
- Arduino IDE configured for ESP32-S3
- NeoPixel LED strip (WS2812B recommended) connected to configured data pin

## Configuration Steps

### 1. Set Your LED Count

Open `potatomelt/src/melty_config.h` and locate the LED configuration section:

```cpp
// ------------ LED Configuration --------------------

#define NEOPIXEL_LED_COUNT 2  // Number of NeoPixel LEDs (0-16 supported)
```

Change the value to match your hardware:

```cpp
#define NEOPIXEL_LED_COUNT 4  // Example: Robot has 4 LEDs
```

**Supported Values**: 0 to 16 LEDs
- **0 LEDs**: All LED functions disabled (no-op)
- **1-16 LEDs**: All LEDs display the same color/pattern

### 2. Customize Status Colors (Optional)

In the same section of `melty_config.h`, you can customize the colors for each status state:

```cpp
// LED Status Colors (RGB values 0-255)

// Ready state (armed and operational)
#define LED_COLOR_READY_R 0
#define LED_COLOR_READY_G 0
#define LED_COLOR_READY_B 255  // Default: Blue

// Low battery warning
#define LED_COLOR_LOW_BATTERY_R 255  // Default: Red
#define LED_COLOR_LOW_BATTERY_G 0
#define LED_COLOR_LOW_BATTERY_B 0

// Controller warnings (stale/disconnected)
#define LED_COLOR_CONTROLLER_WARNING_R 255  // Default: Red
#define LED_COLOR_CONTROLLER_WARNING_G 0
#define LED_COLOR_CONTROLLER_WARNING_B 0
```

**Example**: Change ready state to green:
```cpp
#define LED_COLOR_READY_R 0
#define LED_COLOR_READY_G 255
#define LED_COLOR_READY_B 0  // Green
```

### 3. Compile and Upload

1. Connect your ESP32-S3 via USB
2. In Arduino IDE: **Sketch → Upload**
3. Wait for compilation and upload to complete

**Compilation will fail** with a clear error if you configure more than 16 LEDs:
```
error: static assertion failed: NEOPIXEL_LED_COUNT must be between 0 and 16
```

### 4. Verify Configuration

On boot, you should see:
1. **Brief white flash** across all configured LEDs (100ms)
2. LEDs turn off (50ms)
3. Normal operation begins

This startup sequence confirms your LED count is configured correctly.

## Common Configurations

### Single Status LED
```cpp
#define NEOPIXEL_LED_COUNT 1
```
**Use Case**: Minimal status indication, conserve weight/space

### Dual LEDs (Default)
```cpp
#define NEOPIXEL_LED_COUNT 2
```
**Use Case**: Original PotatoMelt configuration, left/right indicators

### Quad LEDs
```cpp
#define NEOPIXEL_LED_COUNT 4
```
**Use Case**: Enhanced visibility, front/back/left/right indicators

### Full Ring (8-12 LEDs)
```cpp
#define NEOPIXEL_LED_COUNT 8
```
**Use Case**: Ring of LEDs around robot perimeter for maximum visibility

### Disabled LEDs
```cpp
#define NEOPIXEL_LED_COUNT 0
```
**Use Case**: Minimal configuration, no visual status (not recommended for safety)

## LED Status Indicators

| Robot State | LED Behavior | Default Color |
|-------------|--------------|---------------|
| **Boot** | Brief flash (150ms total) | White |
| **No Controller** | Rapid blink (200ms on, 800ms off) | Red |
| **Controller Stale** | Slow blink (800ms on, 200ms off) | Red |
| **Ready** | Solid | Blue |
| **Low Battery** | Solid | Red |
| **Spinning** | Gradient based on heading | Red→Yellow→Green |

All LEDs display the same color simultaneously - no per-LED control in this feature.

## Troubleshooting

### LEDs Don't Light Up
- **Check physical connections**: Data pin must match `NEOPIXEL_PIN` in `melty_config.h`
- **Check power**: NeoPixels require separate 5V power supply for strips >2 LEDs
- **Verify LED count**: Physical strip must have at least `NEOPIXEL_LED_COUNT` LEDs

### Compilation Fails
```
error: static assertion failed: NEOPIXEL_LED_COUNT must be between 0 and 16
```
**Solution**: Reduce `NEOPIXEL_LED_COUNT` to 16 or less

### Only Some LEDs Light Up
- **Physical strip has fewer LEDs than configured**: Only available LEDs will light
- **Solution**: Match `NEOPIXEL_LED_COUNT` to your actual hardware

### Extra LEDs Stay Dark
- **Physical strip has more LEDs than configured**: Extra LEDs remain off
- **Solution**: Increase `NEOPIXEL_LED_COUNT` if desired, or leave as-is

### Wrong Colors
- **NeoPixel variant incompatibility**: Some strips use RGB instead of GRB order
- **Solution**: Adjust color mappings in `write_pixel()` if needed (advanced)

### No Startup Flash
- **LED count is 0**: All LED functions are disabled
- **Solution**: Set `NEOPIXEL_LED_COUNT` to at least 1

## Hardware Wiring Reference

```
ESP32-S3                    NeoPixel Strip
┌─────────┐                 ┌────────────┐
│         │                 │            │
│  GPIO17 ├────────────────>│ Data In    │
│  (PIN)  │                 │            │
│         │                 │            │
│    GND  ├─────────┬──────>│ GND        │
│         │         │       │            │
└─────────┘         │       │ 5V Power   │<── External 5V supply
                    │       │            │
                    │       └────────────┘
                    │
              ┌─────┴──────┐
              │  Power GND │
              └────────────┘
```

**Important**: 
- Data pin is defined by `NEOPIXEL_PIN` in `melty_config.h` (default: GPIO 17)
- NeoPixel strips require 5V power (ESP32 can't supply enough current)
- Share ground between ESP32 and LED power supply

## Performance Notes

- **Compilation**: No runtime overhead, all sizing is compile-time
- **Memory**: Each LED uses 48 bytes (negligible even at 16 LEDs)
- **Execution Time**: LED update scales linearly (~30µs total for 16 LEDs)
- **Battery Impact**: More LEDs = more current draw (budget ~60mA per LED at full white)

## Next Steps

After configuring your LEDs:

1. **Test in Safe Environment**: Verify startup flash and status colors before arming
2. **Calibrate**: Follow main robot calibration procedure
3. **Compete**: Your LEDs now match your robot's configuration!

## Additional Resources

- Full Feature Specification: `specs/001-configurable-neopixel/spec.md`
- Implementation Plan: `specs/001-configurable-neopixel/plan.md`
- Research Notes: `specs/001-configurable-neopixel/research.md`
- Data Model: `specs/001-configurable-neopixel/data-model.md`

## Support

For issues or questions:
1. Check troubleshooting section above
2. Review feature specification for detailed requirements
3. Open issue on project repository
