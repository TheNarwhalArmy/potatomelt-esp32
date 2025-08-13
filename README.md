# PotatoMelt-ESP32

PotatoMelt is a drive controller for Meltybrain (translational drift) robots, running on the ESP32-S3 platform.

PotatoMelt-ESP32 has been modified for my Meltybrain robot.

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

# Credits
Skysdottir, NothingLabs, BlackCatMaxy, Robert K, Mew, Tomash, IrregularJoe
