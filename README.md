# PotatoMelt-ESP32

PotatoMelt is a drive controller for Meltybrain (translational drift) robots, running on the ESP32-S3 platform.

This library is not guaranteed fit for any purpose, and may cause your robot to just explode instead of doing anything useful. You Have Been Warned.

This is based on the fantastic OpenMelt2: https://github.com/nothinglabs/openmelt2

See also my previous version for atmega32/arduino, https://github.com/skysdottir/potatomelt

## Controls

Right trigger: SPIN TIME
Right stick: Turn left/right, translate forwards/backwards (both while spinning and in tank mode)
Left stick up/down: adjust target RPM
X: Reverse spin direction
Dpad left/right: Adjust spin calibration
Dpad up/down: Adjust translation calibration

## LED signals

flashing red: No controller connected
mostly red, flashing off: no recent control inputs detected (failsafe)
Solid blue: Controller connected, ready, in tank mode
Solid red: Controller connected, battery depleted
Drawing arcs, green fading to red: Spinning, displaying battery charge

## Configuration

### Neopixel LED Count

The number of neopixel LEDs in the strip can be configured by changing the `NEOPIXEL_LED_COUNT` value in `potatomelt/src/melty_config.h`. The default is 2 LEDs for backward compatibility.

```cpp
#define NEOPIXEL_LED_COUNT 2    // Change this to your desired LED count
```

All LEDs in the strip will display the same color and pattern. Supported values are any positive integer, though practical limits depend on your ESP32's memory and power supply capacity.

## Just In Case

The arduino project build directory on Windows defaults to: C:\Users\{user}\AppData\Local\Temp\arduino\sketches

# Credits

With apologies to my reviewers:
Tomash and IrregularJoe

And thanks to those who have contributed and inspired:
NothingLabs, BlackCatMaxy, Robert K, and Mew