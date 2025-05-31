#include <Arduino.h>
// #include <driver/rmt.h> // No longer needed for FastLED
#include "led.h"
#include "../melty_config.h" // For NUM_LEDS, NEOPIXEL_PIN

// RMT specific global variables are removed.

LED::LED() {
    // Initialize FastLED
    // Using WS2812B as a common type and GRB color order.
    // NEOPIXEL_PIN and NUM_LEDS come from melty_config.h
    FastLED.addLeds<WS2812B, NEOPIXEL_PIN, GRB>(leds_array, NUM_LEDS);
    FastLED.setBrightness(50); // Set a default brightness (0-255)
}

void LED::leds_on_ready() {
    // Sets all LEDs to blue
    leds_on_rgb(0, 0, 255);
}

void LED::leds_on_low_battery() {
    // Sets all LEDs to red
    leds_on_rgb(255, 0, 0);
}

void LED::leds_on_controller_stale() {
    // Flashing red: 800ms on, 200ms off in a 1-second cycle
    long now = millis();
    if ((now % 1000) < 800) {
        leds_on_rgb(255, 0, 0); // Red
    } else {
        leds_off(); // Off
    }
}

void LED::leds_on_no_controller() {
    // Quick flashing red: 200ms on, 800ms off in a 1-second cycle
    long now = millis();
    if ((now % 1000) < 200) {
        leds_on_rgb(255, 0, 0); // Red
    } else {
        leds_off(); // Off
    }
}

void LED::leds_on_gradient(int color) {
    // This function, based on its original implementation, sets a uniform color
    // across the strip, where the color itself is a gradient from green to red.
    // 'color' is expected to be 0-100.
    int r, g, b;
    b = 0; // Blue is always off for this gradient

    if (color < 0) color = 0;
    if (color > 100) color = 100;

    if (color <= 50) { // From green (at color=0) to yellow (at color=50)
        r = map(color, 0, 50, 0, 255); // Red increases
        g = 255;                       // Green is full
    } else { // From yellow (at color=50) to red (at color=100)
        r = 255;                       // Red is full
        g = map(color, 50, 100, 255, 0); // Green decreases
    }
    leds_on_rgb(r, g, b);
}

void LED::leds_off() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds_array[i] = CRGB::Black;
    }
    FastLED.show();
}

void LED::leds_on_rgb(int red, int green, int blue) {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds_array[i].setRGB(red, green, blue);
    }
    FastLED.show();
}

// New method to display a column of image data
void LED::display_image_column(CRGB* column_data) {
    if (column_data == nullptr) {
        leds_off(); // Turn off if data is invalid
        return;
    }
    // Ensure NUM_LEDS from melty_config.h is used for sizing.
    // leds_array is already sized with NUM_LEDS.
    memcpy(leds_array, column_data, NUM_LEDS * sizeof(CRGB));
    FastLED.show();
}

// The old LED::write_pixel() method is entirely removed.
// A final newline character is added below.