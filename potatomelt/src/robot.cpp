#include "robot.h"
#include "melty_config.h"

Robot::Robot():
    motor1(MOTOR_1_PIN, MOTOR_1_RMT),
    motor2(MOTOR_2_PIN, MOTOR_2_RMT)  {
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
    motor1.begin(DSHOT300);
    motor2.begin(DSHOT300);
}