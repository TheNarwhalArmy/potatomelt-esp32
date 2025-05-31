#include "../melty_config.h"
#include <FastLED.h>                // Standard include path

class LED {
    public:
        LED();

        void leds_on_ready();
        void leds_on_low_battery();
        void leds_on_controller_stale();
        void leds_on_no_controller();

        void leds_on_gradient(int color);
        void leds_off();

        // New method for displaying image columns
        void display_image_column(CRGB* column_data);

    private:
        CRGB leds_array[NUM_LEDS]; // Array to hold LED color data

        void leds_on_rgb(int red, int green, int blue);
        // write_pixel() is removed as FastLED.show() handles this.
};