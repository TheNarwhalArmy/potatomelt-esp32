#include <Arduino.h>
#include <math.h>
#include "accelerometer.h"
#include "imu.h"
#include "storage.h"
#include "../melty_config.h"

Accelerometer lis1;
Accelerometer lis2;

// todo - spin trim

IMU::IMU() {
}

void IMU::init() {
    lis1.init(0x18);
    lis2.init(0x19);
    delay(20); // short pause for accelerometer warmup - we get weird results if we just dive right in
    set_z_offset();
}

void IMU::set_z_offset() {
    //todo - save offset into config?
    for(int i = 0; i < 60; i++) {
        lis1.sample_offset();
        lis2.sample_offset();
        delay(1);
    }
}

// to be called every hit of loop()
// todo - rethink this
void IMU::poll() {
    float avg_z_g = (lis1.get_z_accel() + lis2.get_z_accel()) / 2;
    
    z_accel_buffer *= 0.5;
    z_accel_buffer += (0.5 * avg_z_g);
}

// todo - factor this into tank mode controls so we drive normally when inverted
bool IMU::get_inverted() {
    return z_accel_buffer < 0.0;
}

float IMU::get_rpm() {
    float avg_g = (lis1.get_xy_accel() + lis2.get_xy_accel()) / 2;
    float rpm = fabs(avg_g) * 89445.0f;
    rpm = rpm / ACCELEROMETER_HARDWARE_RADIUS_CM;
    rpm = sqrt(rpm);
    
    return rpm;
}