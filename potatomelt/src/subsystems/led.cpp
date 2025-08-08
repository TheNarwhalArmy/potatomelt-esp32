
#include <Arduino.h>
#include <driver/rmt.h>
#include <cstring>
#include "led.h"
#include "../melty_config.h"

LED::LED(int num_leds) : num_leds_(num_leds) {
    pixel_color_ = new uint8_t[3 * num_leds_];
    override_ = new bool[num_leds_];
    override_color_ = new uint8_t[3 * num_leds_];
    led_data_ = new rmt_item32_t[num_leds_ * 8 * 3];
    main_red_ = main_green_ = main_blue_ = 0;
    clear_overrides();

    rmt_config_t rmt_cfg = RMT_DEFAULT_CONFIG_TX(NEOPIXEL_PIN, NEOPIXEL_RMT);
    rmt_cfg.clk_div = 8; // slow us down to 10mhz
    rmt_config(&rmt_cfg);
    rmt_driver_install(rmt_cfg.channel, 0, 0);
}

void LED::clear_overrides() {
    for (int i = 0; i < num_leds_; ++i) {
        override_[i] = false;
        override_color_[3*i+0] = 0;
        override_color_[3*i+1] = 0;
        override_color_[3*i+2] = 0;
    }
}

void LED::set_led_rgb(int led_index, int red, int green, int blue) {
    if (led_index < 0 || led_index >= num_leds_) return;
    override_[led_index] = true;
    // NeoPixels use GRB order
    override_color_[3*led_index+0] = green;
    override_color_[3*led_index+1] = red;
    override_color_[3*led_index+2] = blue;
    write_pixel();
}

void LED::leds_on_ready() {
    leds_on_rgb(0, 0, 255);
}

void LED::leds_on_low_battery() {
    leds_on_rgb(255, 0, 0);
}

void LED::leds_on_controller_stale() {
    long now = millis();
    now /= 100;
    if ((now % 10) < 8) {
        leds_on_rgb(255, 0, 0);
    } else {
        leds_off();
    }
}

void LED::leds_on_no_controller() {
    long now = millis();
    now /= 100;
    if ((now % 10) < 2) {
        leds_on_rgb(255, 0, 0);
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
    main_red_ = main_green_ = main_blue_ = 0;
    for (int i = 0; i < num_leds_; ++i) {
        if (!override_[i]) {
            pixel_color_[3*i+0] = 0;
            pixel_color_[3*i+1] = 0;
            pixel_color_[3*i+2] = 0;
        }
    }
    write_pixel();
}

void LED::leds_on_rgb(int red, int green, int blue) {
    // Store main color
    main_red_ = red;
    main_green_ = green;
    main_blue_ = blue;
    // Set all non-overridden LEDs to this color (GRB order)
    for (int i = 0; i < num_leds_; ++i) {
        if (!override_[i]) {
            pixel_color_[3*i+0] = green;
            pixel_color_[3*i+1] = red;
            pixel_color_[3*i+2] = blue;
        }
    }
    write_pixel();
}

void LED::write_pixel() {
    // Compose the pixel_color_ array: overridden LEDs use override_color_, others use pixel_color_
    for (int i = 0; i < num_leds_; ++i) {
        if (override_[i]) {
            // Copy override color to pixel_color_
            pixel_color_[3*i+0] = override_color_[3*i+0];
            pixel_color_[3*i+1] = override_color_[3*i+1];
            pixel_color_[3*i+2] = override_color_[3*i+2];
        }
    }
    int index = 0;
    for (int i = 0; i < num_leds_ * 3; ++i) {
        uint8_t value = pixel_color_[i];
        for (int bit = 7; bit >= 0; bit--) {
            if ((value >> bit) & 1) {
                led_data_[index].level0 = 1;
                led_data_[index].duration0 = 8;
                led_data_[index].level1 = 0;
                led_data_[index].duration1 = 4;
            } else {
                led_data_[index].level0 = 1;
                led_data_[index].duration0 = 4;
                led_data_[index].level1 = 0;
                led_data_[index].duration1 = 8;
            }
            index++;
        }
    }
    rmt_write_items(NEOPIXEL_RMT, led_data_, num_leds_ * 8 * 3, false);
}