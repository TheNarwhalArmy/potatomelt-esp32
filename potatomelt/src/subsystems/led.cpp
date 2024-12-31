#include <Arduino.h>
#include "led.h"
#include "melty_config.h"

rmt_data_t rmt_data[6 * 8];
uint8_t pixel_color[6]; 

LED::LED() {
    rmtInit(NEOPIXEL_PIN, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, 10000000);
}

void LED::leds_on(Status status) {
    switch (status) {
        case TANK:
            leds_on_rgb(0, 0, 255);
            break;
        case BATTERY:
            leds_on_rgb(255, 0, 0);
            break;
        case LOS:
        default:
            long now = millis();
            now /= 100;
            if ((now % 10) < 2) {
                leds_on_rgb(255, 0, 0);
            } else {
                leds_off();
            }
            break;
    }
}

void LED::leds_on_gradient(int color) {
    int green = (color > 50) ? 255 : 255*color/50;
    int red = (color < 50) ? 255 : 255*(100-color)/50;
    leds_on_rgb(red, green, 0);
}

void LED::leds_off() {
    pixel_color[0] = pixel_color[1] = pixel_color[2] = 0;
    pixel_color[3] = pixel_color[4] = pixel_color[5] = 0;
    write_pixel();
}

void LED::leds_on_rgb(int red, int green, int blue) {
    // neopixels usually use GRB addressing rather than RGB
    pixel_color[0] = pixel_color[3] = green;
    pixel_color[1] = pixel_color[4] = red;
    pixel_color[2] = pixel_color[5] = blue;
    write_pixel();
}

void LED::write_pixel() {
    int index = 0;
    for (auto chan : pixel_color) {
      uint8_t value = chan;
      for (int bit = 7; bit >= 0; bit--) {
        if ((value >> bit) & 1) {
          rmt_data[index].level0 = 1;
          rmt_data[index].duration0 = 8;
          rmt_data[index].level1 = 0;
          rmt_data[index].duration1 = 4;
        } else {
          rmt_data[index].level0 = 1;
          rmt_data[index].duration0 = 4;
          rmt_data[index].level1 = 0;
          rmt_data[index].duration1 = 8;
        }
        index++;
      }
    }
    rmtWriteAsync(NEOPIXEL_PIN, rmt_data, 6 * 8);
  }