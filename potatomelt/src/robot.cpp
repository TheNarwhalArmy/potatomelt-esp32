#include "robot.h"
#include "melty_config.h"

Robot::Robot() {
}

void Robot::update_loop(control_parameters_t* params) {
    imu.poll();

    int led_status = (imu.get_inverted()) ? 0 : 100;

    leds.leds_on_gradient(led_status);
}

float Robot::get_z_buffer() {
    return imu.z_accel_buffer;
}

void Robot::init() {
    imu.init();
    motor1.init(MOTOR_1_PIN);
    motor2.init(MOTOR_2_PIN);
}