#include <Arduino.h>
#include "accelerometer.h"
#include "imu.h"

Accelerometer lis1;
Accelerometer lis2;

IMU::IMU() {
}

void IMU::init() {
    lis1.init(0x18);
    lis2.init(0x19);
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
void IMU::poll() {
    float avg_z_g = (lis1.get_z_accel() + lis2.get_z_accel()) / 2;
    
    z_accel_buffer *= 0.5;
    z_accel_buffer += (0.5 * avg_z_g);
}

bool IMU::get_inverted() {
    return z_accel_buffer < 0.0;
}

int IMU::get_rpm() {
    //todo - no spin for you!
    return 0;
}