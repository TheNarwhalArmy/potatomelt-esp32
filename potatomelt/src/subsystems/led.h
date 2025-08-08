#include "../melty_config.h"

class LED {
public:
    // num_leds: number of NeoPixel LEDs in the chain (default NEOPIXEL_COUNT from config)
    LED(int num_leds = NEOPIXEL_COUNT);

    void leds_on_ready();
    void leds_on_low_battery();
    void leds_on_controller_stale();
    void leds_on_no_controller();

    void leds_on_gradient(int color);
    void leds_off();

    // Override a specific LED's color (index, r, g, b)
    void set_led_rgb(int led_index, int red, int green, int blue);
    // Clear all overrides (all LEDs will mirror the main color again)
    void clear_overrides();

private:
    void leds_on_rgb(int red, int green, int blue);
    void write_pixel();

    int num_leds_;
    uint8_t* pixel_color_; // 3*num_leds_ (GRB for each LED)
    bool* override_;      // true if LED is overridden
    uint8_t* override_color_; // 3*num_leds_ (GRB for each LED)
    rmt_item32_t* led_data_; // num_leds_*8*3
    int main_red_, main_green_, main_blue_;
};