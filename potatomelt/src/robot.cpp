#include "robot.h"
#include "melty_config.h"
#include "subsystems/ImageDisplay.h" // Include for ImageDisplay class

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

// Updated signature for update_loop
void Robot::update_loop(robot_status state, spin_control_parameters_t* spin_params, tank_control_parameters_t* tank_params, bool image_mode_active, ImageDisplay* img_display) {
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
            break;
        case SPINNING:
            // Pass new parameters to spin
            spin(spin_params, image_mode_active, img_display);
            break;
    }
}

// Updated signature for spin
void Robot::spin(spin_control_parameters_t* params, bool image_mode_active, ImageDisplay* img_display) {
    long time_spent_this_rotation_us = micros() - rotation_started_at_us;

    // Ensure time_spent_this_rotation_us stays within one rotation interval for calculations
    // This also handles the reset of rotation_started_at_us for the next full rotation.
    if (params->rotation_interval_us > 0) { // Avoid division by zero or issues if not spinning properly
        if (time_spent_this_rotation_us >= params->rotation_interval_us) {
            // How many full rotations have passed?
            long rotations_passed = time_spent_this_rotation_us / params->rotation_interval_us;
            rotation_started_at_us += rotations_passed * params->rotation_interval_us;
            time_spent_this_rotation_us %= params->rotation_interval_us;
        }
    } else {
        // Not rotating or interval is zero, reset time spent to avoid issues
        time_spent_this_rotation_us = 0;
    }


    double throttle_offset = 0;

    // translation math time - first, how far into this phase of rotation are we?
    // A phase is considered half a rotation for bi-directional motor control.
    if (params->rotation_interval_us > 0) { // Only calculate phase if actually rotating
        long half_rotation_us = params->rotation_interval_us / 2;
        if (half_rotation_us > 0) { // Ensure half_rotation_us is not zero
            long micros_into_phase = time_spent_this_rotation_us % half_rotation_us;
            float phase_progress = (float)micros_into_phase / (float)half_rotation_us;

            // Parabolic approximation of half a sine wave (Y= -4X(X-1) for X from 0 to 1)
            float phase_offset_fraction = -4.0f * phase_progress * (phase_progress - 1.0f);
            throttle_offset = (double) (phase_offset_fraction * params->max_throttle_offset);
        }
    }


    // Motor control logic based on current half of rotation
    if (params->rotation_interval_us > 0) { // Check to prevent issues if rotation_interval_us is zero
        long half_rotation_point = params->rotation_interval_us / 2;
        // Determine if in the first or second half of the rotation for motor direction
        if (time_spent_this_rotation_us < half_rotation_point) { // First half
            motor1.sendThrottleValue(perk2dshot(params->throttle_perk + throttle_offset));
            motor2.sendThrottleValue(perk2dshot(params->throttle_perk - throttle_offset));
        } else { // Second half
            motor1.sendThrottleValue(perk2dshot(params->throttle_perk - throttle_offset));
            motor2.sendThrottleValue(perk2dshot(params->throttle_perk + throttle_offset));
        }
    } else {
        motors_stop(); // If not rotating, stop motors
    }

    // Conditional LED logic
    if (image_mode_active && img_display != nullptr && img_display->is_loaded()) {
        // Image display mode
        if (params->rotation_interval_us > 0) {
            float rotation_progress = (float)time_spent_this_rotation_us / (float)params->rotation_interval_us;
            int image_w = img_display->get_width();
            if (image_w > 0) {
                int current_column = (int)(rotation_progress * image_w) % image_w;
                CRGB column_buffer[NUM_LEDS]; // NUM_LEDS from melty_config.h
                img_display->get_column(current_column, column_buffer);
                leds.display_image_column(column_buffer);
            } else {
                leds.leds_off(); // No image width, turn off LEDs
            }
        } else {
            leds.leds_off(); // Not spinning (rotation_interval_us is 0), turn off LEDs
        }
    } else {
        // Original heading LED logic
        if (params->rotation_interval_us > 0) { // Only display if actually rotating
            if (params->led_start > params->led_stop) {
                // The LED will be on across 0 (e.g., start at 300 degrees, stop at 60 degrees)
                if (time_spent_this_rotation_us >= params->led_start || time_spent_this_rotation_us <= params->led_stop) {
                    leds.leds_on_gradient(params->battery_percent);
                } else {
                    leds.leds_off();
                }
            } else {
                // The LED is on in a contiguous block (e.g., start at 30, stop at 90)
                if (time_spent_this_rotation_us >= params->led_start && time_spent_this_rotation_us <= params->led_stop) {
                    leds.leds_on_gradient(params->battery_percent);
                } else {
                    leds.leds_off();
                }
            }
        } else {
             leds.leds_off(); // Not spinning, turn off LEDs
        }
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

float Robot::get_rpm(int target_rpm) {
    return imu.get_rpm(target_rpm);
}

int Robot::get_battery() {
    return battery.get_percent();
}

void Robot::trim_accel(bool increase, int target_rpm) {
  imu.trim(increase, target_rpm);
}

float Robot::get_accel_trim(int target_rpm) {
  return imu.get_trim(target_rpm);
}

void Robot::init() {
    imu.init();
    motor1.begin(DSHOT300);
    motor2.begin(DSHOT300);
    rotation_started_at_us = micros(); // Initialize rotation start time
}