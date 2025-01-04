#include <Arduino.h>
#include <math.h>
#include "accelerometer.h"
#include "imu.h"
#include "storage.h"
#include "../melty_config.h"

Accelerometer lis1;
Accelerometer lis2;

Storage store;

float accel_correction_factor = 1.0;

// todo - spin trim per-RPM

IMU::IMU() {
}

void IMU::init() {
    lis1.init(0x18);
    lis2.init(0x19);
    store = get_active_store();
    accel_correction_factor = store.get_accel_correction();

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

void IMU::trim(bool increase) {
    accel_correction_factor *= (increase) ? 1.05 : (1.0/1.05);
    store.set_accel_correction(accel_correction_factor);
}

float IMU::get_rpm() {
    float lis1_g = lis1.get_xy_accel();
    float lis2_g = lis2.get_xy_accel();

    float avg_g = (lis1_g + lis2_g) / 2;
    float rpm = fabs(avg_g) * 89445.0f;
    rpm = rpm / ACCELEROMETER_HARDWARE_RADIUS_CM;
    rpm = sqrt(rpm);
    rpm *= accel_correction_factor;

    return rpm;
}

float IMU::get_accel_1_g() {
    return lis1.get_xy_accel();
}

float IMU::get_accel_2_g() {
    return lis2.get_xy_accel();
}