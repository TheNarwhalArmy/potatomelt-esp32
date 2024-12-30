#include "robot.h"

void Robot::init() {
    leds.init();
    led_status_temp = 100;
}

void Robot::update_loop(control_parameters_t* params) {
    led_status_temp -= 1;
    if (led_status_temp < 0) {
        led_status_temp = 100;
    }

    leds.leds_on_gradient(led_status_temp);
}