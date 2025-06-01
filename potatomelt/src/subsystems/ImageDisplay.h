#ifndef IMAGE_DISPLAY_H
#define IMAGE_DISPLAY_H

#include <Arduino.h>
#include "SPIFFS.h"
#include <FastLED.h>                // Standard include path
#include "../melty_config.h"       // For NUM_LEDS

// Forward declaration if Pngle types were used directly in headers
// For now, pngle interaction is in the .cpp's placeholder.

class ImageDisplay {
public:
    // Member Variables
    bool image_loaded;
    int image_width;
    int image_height; // Should be NUM_LEDS (20) - this is the buffer height
    int actual_png_height; // Actual height of the loaded PNG
    CRGB* pixel_data; // Decoded pixel data, sized image_width * image_height

    // Constructor
    ImageDisplay();

    // Destructor
    ~ImageDisplay();

    // Public Methods
    bool load_image(const char* filename);
    CRGB get_pixel(int x, int y);
    void get_column(int col_index, CRGB* column_buffer); // Buffer should be NUM_LEDS in size
    int get_width();
    bool is_loaded();

private:
    // Private helper methods if any (e.g., for actual PNG decoding later)

    // Pngle callback functions
    // pngle_t is a typedef defined in pngle.h, which is included by ImageDisplay.cpp
    static void pngle_init_callback(pngle_t* pngle, uint32_t w, uint32_t h);
    static void pngle_draw_callback(pngle_t* pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint8_t rgba[4]);
    static void pngle_done_callback(pngle_t* pngle);
};

#endif // IMAGE_DISPLAY_H
