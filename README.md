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

Blinking red: No controller connected or no recent controls received (failsafe)
Solid blue: Controller connected, ready, in tank mode
Solid red: Controller connected, battery depleted
Drawing arcs, green fading to red: Spinning, displaying battery charge