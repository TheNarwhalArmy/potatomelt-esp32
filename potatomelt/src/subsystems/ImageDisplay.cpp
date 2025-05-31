#include "ImageDisplay.h"
#include "../melty_config.h" // For NUM_LEDS
#include <pngle.h>              // Standard include path

// Constructor
ImageDisplay::ImageDisplay() {
    image_loaded = false;
    image_width = 0;
    image_height = 0; // This will be NUM_LEDS (buffer height)
    actual_png_height = 0; // Actual height of the loaded PNG
    pixel_data = nullptr;
}

// Destructor
ImageDisplay::~ImageDisplay() {
    if (pixel_data != nullptr) {
        delete[] pixel_data;
        pixel_data = nullptr;
    }
}

// --- Pngle Static Callback Implementations ---
void ImageDisplay::pngle_init_callback(pngle_t* pngle, uint32_t w, uint32_t h) {
    ImageDisplay* self = (ImageDisplay*)pngle_get_user_data(pngle);
    if (self == nullptr) {
        Serial.println("Pngle Error: self context is null in init_callback.");
        return;
    }

    self->image_width = w;
    self->image_height = NUM_LEDS; // Fixed buffer height, NUM_LEDS from melty_config.h
    self->actual_png_height = h;   // Store the actual height of the PNG

    // Free old data if any
    if (self->pixel_data != nullptr) {
        delete[] self->pixel_data;
        self->pixel_data = nullptr;
    }

    self->pixel_data = new CRGB[self->image_width * self->image_height];
    if (self->pixel_data == nullptr) {
        Serial.println("Failed to allocate pixel_data for PNG"); // Error message from prompt
        self->image_loaded = false; // As per prompt
        return;
    }

    // Initialize pixel_data to black
    for (int i = 0; i < self->image_width * self->image_height; ++i) {
        self->pixel_data[i] = CRGB::Black;
    }

    Serial.printf("Pngle init: PNG dimensions %u x %u. Buffer %d x %d\n", w, h, self->image_width, self->image_height); // %u for uint32_t
}

void ImageDisplay::pngle_draw_callback(pngle_t* pngle, uint32_t x, uint32_t y, uint32_t pixel_w, uint32_t pixel_h, const uint8_t rgba[4]) {
    ImageDisplay* self = (ImageDisplay*)pngle_get_user_data(pngle);
    if (self == nullptr || self->pixel_data == nullptr) {
        return;
    }

    for (uint32_t iy = 0; iy < pixel_h; ++iy) {
        for (uint32_t ix = 0; ix < pixel_w; ++ix) {
            uint32_t current_y = y + iy;
            uint32_t current_x = x + ix;
            if (current_y < (uint32_t)self->image_height && current_x < (uint32_t)self->image_width) { // Draw only if within our buffer bounds
                self->pixel_data[current_y * self->image_width + current_x] = CRGB(rgba[0], rgba[1], rgba[2]);
            }
        }
    }
}

void ImageDisplay::pngle_done_callback(pngle_t* pngle) {
    ImageDisplay* self = (ImageDisplay*)pngle_get_user_data(pngle);
    if (self == nullptr) {
        Serial.println("Pngle Error: self context is null in done_callback.");
        return;
    }

    if (self->pixel_data != nullptr) { // Only set loaded if init was successful
       self->image_loaded = true;
       Serial.println("Pngle processing done. Image loaded.");
    } else {
       Serial.println("Pngle processing done, but image not marked loaded due to earlier errors (e.g., memory allocation).");
       self->image_loaded = false;
    }
}


// --- Updated Public Methods ---
bool ImageDisplay::load_image(const char* filename) {
    this->image_loaded = false;
    // Constructor should ensure pixel_data is nullptr initially.
    // If load_image can be called multiple times, destructor logic for pixel_data is in init_callback.

    if (!SPIFFS.exists(filename)) {
        Serial.printf("ImageDisplay: File %s does not exist.\n", filename);
        return false;
    }

    File file = SPIFFS.open(filename, "r");
    if (!file || file.isDirectory() || !file.size()) {
        Serial.printf("ImageDisplay: Failed to open file %s or file is empty/directory.\n", filename);
        if (file) file.close();
        return false;
    }

    Serial.printf("ImageDisplay: Opened file %s, size %lu bytes. Attempting to decode with Pngle.\n", filename, file.size());

    pngle_t* pngle = pngle_new();
    if (!pngle) {
        Serial.println("Failed to create Pngle instance"); // Error message from prompt
        file.close();
        return false;
    }

    pngle_set_user_data(pngle, this);
    pngle_set_init_callback(pngle, ImageDisplay::pngle_init_callback);
    pngle_set_draw_callback(pngle, ImageDisplay::pngle_draw_callback);
    pngle_set_done_callback(pngle, ImageDisplay::pngle_done_callback);

    const size_t BUFFER_SIZE = 1024; // Can be tuned
    uint8_t buf[BUFFER_SIZE];
    int len = 0;
    // int fed_total = 0; // For debugging

    while ((len = file.read(buf, BUFFER_SIZE)) > 0) {
        int fed = pngle_feed(pngle, buf, len);
        if (fed < 0) {
            Serial.printf("Pngle feed error: %s\n", pngle_error_msg(pngle)); // pngle_error_msg is correct
            // this->image_loaded will remain false or be set by callbacks
            break;
        }
        // fed_total += fed; // For debugging
    }
    // Serial.printf("Total bytes fed to Pngle: %d\n", fed_total); // For debugging

    file.close(); // Close file regardless of Pngle outcome (after loop or break)

    // Check Pngle state after feeding all data
    // If an error occurred during feed, or if done_callback wasn't reached,
    // image_loaded might still be false.
    // The done_callback is responsible for setting image_loaded = true on success if pixel_data is valid.
    if (pngle_get_state(pngle) == PNGLE_STATE_ERROR && this->image_loaded) {
        // If pngle is in error state, ensure image_loaded is false, even if done_callback somehow set it true
        // (e.g. if error happened after done_callback, though unlikely with current pngle_feed structure)
        Serial.printf("ImageDisplay: Pngle ended in error state (%s), ensuring image_loaded is false.\n", pngle_error_msg(pngle));
        this->image_loaded = false;
    }

    pngle_destroy(pngle); // Destroy Pngle instance

    // Final check based on pixel_data status (which init_callback manages)
    if (this->pixel_data == nullptr) {
      this->image_loaded = false; // Should have been caught by init or done callbacks
    }

    if (!this->image_loaded) {
         Serial.printf("ImageDisplay: Failed to load PNG %s.\n", filename);
    }

    return this->image_loaded;
}

CRGB ImageDisplay::get_pixel(int x, int y) {
    if (!image_loaded || pixel_data == nullptr || x < 0 || x >= image_width || y < 0 || y >= image_height) {
        return CRGB::Black;
    }
    return pixel_data[y * image_width + x];
}

void ImageDisplay::get_column(int col_index, CRGB* column_buffer) {
    if (column_buffer == nullptr) return;

    for (int i = 0; i < NUM_LEDS; ++i) {
        column_buffer[i] = CRGB::Black;
    }

    if (!image_loaded || pixel_data == nullptr || image_width <= 0) {
        return;
    }

    col_index = col_index % image_width;
    if (col_index < 0) {
        col_index += image_width;
    }

    for (int i = 0; i < this->image_height; ++i) {
        column_buffer[i] = pixel_data[i * this->image_width + col_index];
    }
}

int ImageDisplay::get_width() {
    return (image_loaded && pixel_data != nullptr) ? image_width : 0;
}

bool ImageDisplay::is_loaded() {
    return (image_loaded && pixel_data != nullptr);
}
