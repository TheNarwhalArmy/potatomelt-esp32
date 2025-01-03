#include "robot.h"
#include "melty_config.h"

int perk2dshot(int throttle) {
  if (throttle == 0) {
    return 0;
  }
  
  if (throttle > 0) {
    return min(throttle, 999) + 48;
  }

  // otherwise, throttle_perk must be less than zero
  return min(throttle*-1, 998) + 1049;
}

Robot::Robot():
    motor1(MOTOR_1_PIN, MOTOR_1_RMT),
    motor2(MOTOR_2_PIN, MOTOR_2_RMT)  {
}

void Robot::update_loop(robot_status state, spin_control_parameters_t* spin_params, tank_control_parameters_t* tank_params) {
    switch(state) {
        default:
        case NO_CONTROLLER:
            leds.leds_on_no_controller();
            motors_stop();
            break;
        case CONTROLLER_STALE:
            leds.leds_on_controller_stale();
            motors_stop();
            break;
        case READY:
            leds.leds_on_ready();
            drive_tank(tank_params);
    }
}

void Robot::motors_stop() {
    motor1.sendThrottleValue(0);
    motor2.sendThrottleValue(0);
}

void Robot::drive_tank(tank_control_parameters_t* params) {
    if (abs(params->translate_forback) > CONTROL_TRANSLATE_DEADZONE || abs(params->turn_lr) > CONTROL_TRANSLATE_DEADZONE) {
        int forback = params->translate_forback * TANK_FORBACK_POWER_SCALE;
        int leftright = params->turn_lr * TANK_TURNING_POWER_SCALE;

        motor1.sendThrottleValue(perk2dshot(forback + leftright));
        motor2.sendThrottleValue(perk2dshot(-1 * (forback - leftright)));
    } else {
        motors_stop();
    }
}

float Robot::get_z_buffer() {
    return imu.z_accel_buffer;
}

void Robot::init() {
    imu.init();
    motor1.begin(DSHOT300);
    motor2.begin(DSHOT300);
}