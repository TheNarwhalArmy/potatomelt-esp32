#include <Arduino.h>
#include <driver/rmt.h>
#include "led.h"
#include "../melty_config.h"

// Compile-time validation of LED count
// Valid range: 1-16 LEDs
// - 1-16 LEDs: All LEDs display the same color/pattern
// - 0 LEDs or >16 LEDs: Compilation fails with this error
static_assert(NEOPIXEL_LED_COUNT >= 1 && NEOPIXEL_LED_COUNT <= 16, 
              "NEOPIXEL_LED_COUNT must be between 1 and 16");

// LED data buffers sized by NEOPIXEL_LED_COUNT configuration
// led_data: RMT timing buffer (3 bytes × 8 bits × 4 bytes/item = 96 bytes per LED)
// pixel_color: RGB color buffer in GRB format (3 bytes per LED)
rmt_item32_t led_data[NEOPIXEL_LED_COUNT * 3 * 8];
uint8_t pixel_color[NEOPIXEL_LED_COUNT * 3]; 

LED::LED() {
    // Constructor does minimal initialization - no delays allowed here
}

void LED::init() {
    // Initialize RMT peripheral for NeoPixel control
    rmt_config_t rmt_cfg = RMT_DEFAULT_CONFIG_TX(NEOPIXEL_PIN, NEOPIXEL_RMT);

    rmt_cfg.clk_div = 8; // slow us down to 10mhz

    rmt_config(&rmt_cfg);

    rmt_driver_install(rmt_cfg.channel, 0, 0);

    // Startup diagnostic: Sequential LED illumination to confirm LED count
    // LEDs turn on one by one over 1 second, then stay lit for 1 additional second
    // First LED: Max green, Last LED: Max blue
    // Middle LEDs: White with brightness scaling from 10% to 90%
    
    // Clear all LEDs first
    leds_off();
    
    // Calculate delay between each LED turning on
    int delay_per_led = 1000 / NEOPIXEL_LED_COUNT;
    
    // Turn on LEDs sequentially
    for (int led = 0; led < NEOPIXEL_LED_COUNT; led++) {
        int led_index = led * 3;
        
        if (led == 0) {
            // First LED: Max green
            pixel_color[led_index + 0] = 255;  // G
            pixel_color[led_index + 1] = 0;    // R
            pixel_color[led_index + 2] = 0;    // B
        } else if (led == NEOPIXEL_LED_COUNT - 1) {
            // Last LED: Max blue
            pixel_color[led_index + 0] = 0;    // G
            pixel_color[led_index + 1] = 0;    // R
            pixel_color[led_index + 2] = 255;  // B
        } else {
            // Middle LEDs: White with brightness scaling from 10% to 90%
            int middle_count = NEOPIXEL_LED_COUNT - 2;
            int middle_index = led - 1;  // 0-based index among middle LEDs
            
            // Calculate brightness: 10% + (80% * progress)
            float progress = (middle_count == 1) ? 0.5f : (float)middle_index / (float)(middle_count - 1);
            int brightness = 25 + (int)(204.0f * progress);  // 25 (10% of 255) to 229 (90% of 255)
            
            pixel_color[led_index + 0] = brightness;  // G
            pixel_color[led_index + 1] = brightness;  // R
            pixel_color[led_index + 2] = brightness;  // B
        }
        
        write_pixel();
        delay(delay_per_led);
    }
    
    // Keep all LEDs lit for an additional 1 second
    delay(1000);
    
    // Turn off all LEDs
    leds_off();
    delay(50);
}

void LED::leds_on_ready() {
    leds_on_rgb(LED_COLOR_READY_R, LED_COLOR_READY_G, LED_COLOR_READY_B);
}

void LED::leds_on_low_battery() {
    leds_on_rgb(LED_COLOR_LOW_BATTERY_R, LED_COLOR_LOW_BATTERY_G, LED_COLOR_LOW_BATTERY_B);
}

void LED::leds_on_controller_stale() {
    long now = millis();
        now /= 100;
        if ((now % 10) < 8) {
            leds_on_rgb(LED_COLOR_CONTROLLER_WARNING_R, LED_COLOR_CONTROLLER_WARNING_G, LED_COLOR_CONTROLLER_WARNING_B);
        } else {
            leds_off();
        }
}

void LED::leds_on_no_controller() {
    long now = millis();
        now /= 100;
        if ((now % 10) < 2) {
            leds_on_rgb(LED_COLOR_CONTROLLER_WARNING_R, LED_COLOR_CONTROLLER_WARNING_G, LED_COLOR_CONTROLLER_WARNING_B);
        } else {
            leds_off();
        }
}

void LED::leds_on_gradient(int color) {
    int green = (color > 50) ? 255 : 255*color/50;
    int red = (color < 50) ? 255 : 255*(100-color)/50;
    leds_on_rgb(red, green, 0);
}

void LED::leds_off() {
    // Clear all configured LEDs
    for (int i = 0; i < NEOPIXEL_LED_COUNT * 3; i++) {
        pixel_color[i] = 0;
    }
    write_pixel();
}

void LED::leds_on_rgb(int red, int green, int blue) {
    // neopixels usually use GRB addressing rather than RGB
    // Set all configured LEDs to the same color
    for (int i = 0; i < NEOPIXEL_LED_COUNT; i++) {
        pixel_color[i * 3 + 0] = green;  // G
        pixel_color[i * 3 + 1] = red;    // R
        pixel_color[i * 3 + 2] = blue;   // B
    }
    write_pixel();
}

void LED::write_pixel() {
    int index = 0;
    for (auto chan : pixel_color) {
      uint8_t value = chan;
      for (int bit = 7; bit >= 0; bit--) {
        if ((value >> bit) & 1) {
          led_data[index].level0 = 1;
          led_data[index].duration0 = 8;
          led_data[index].level1 = 0;
          led_data[index].duration1 = 4;
        } else {
          led_data[index].level0 = 1;
          led_data[index].duration0 = 4;
          led_data[index].level1 = 0;
          led_data[index].duration1 = 8;
        }
        index++;
      }
    }

    rmt_write_items(NEOPIXEL_RMT, led_data, NEOPIXEL_LED_COUNT * 3 * 8, false);
  }