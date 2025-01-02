#include "accelerometer.h"
#include "../lib/SparkFun_LIS331_ESP32.h"

Accelerometer::Accelerometer() { }

void Accelerometer::init(int addr) {
    lis.setI2CAddr(addr);
    lis.begin(LIS331ESP::USE_I2C);
    lis.setFullScale(LIS331ESP::HIGH_RANGE);
}

// Assumption: We're only calling this when the bot is at rest, right-side-up, in a 1g environment.
void Accelerometer::sample_offset() {
    int16_t x, y, z;
    lis.readAxes(x, y, z);

    float zg = lis.convertToG(400, z);

    sample_count++;
    summed_samples += zg;
    z_offset = summed_samples/sample_count - 1.0;
}

float Accelerometer::get_z_accel() {
    int16_t x, y, z;
    lis.readAxes(x, y, z);
    float zg = lis.convertToG(400, z);
    return zg - z_offset;
}
