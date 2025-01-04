#include <PID_v1.h>
#include <Preferences.h>
#include <Wire.h>
#include "src/robot.h"
#include "src/melty_config.h"
#include "src/controller.h"
#include "src/subsystems/storage.h"

TaskHandle_t hotloop;

Robot robot;

robot_status state;
spin_control_parameters_t control_params;
tank_control_parameters_t tank_params;

long last_logged_at = 0;

// Variables for the PID - it doesn't take args directly, just gets pointers to these
double pid_current_rpm = 0.0; // Input to the PID: The current RPM
double pid_target_rpm = 0.0;  // Setpoint for the PID: The target RPM
double pid_throttle_output = 0.0; // Output from the PID: How hard to run the throttle

// We're using a PID to control motor power, to chase a RPM set by the throttle channel
PID throttle_pid(&pid_current_rpm, &pid_throttle_output, &pid_target_rpm, PID_KP, PID_KI, PID_KD, P_ON_E, DIRECT);

// todo - translation trim

// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);
    Serial.println("PotatoMelt startup");

    // set up I2C
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    // init storage
    get_active_store().init();

    // start the robot subsystems
    robot.init();

    // start the control interface
    BP32.setup(&on_connected_controller, &on_disconnected_controller);

    // and start the hot loop - it'll be managing LEDs and motors
    xTaskCreatePinnedToCore(
        hotloopFN, // the function
        "hotloop", // name the task
        10000,     // stack depth
        NULL,      // params
        1,         // priority
        &hotloop,  // task handle (if we want to interact with the task)
        0          // core affinity
    );
}

// This function is the core of the control loop
// it calculates all the parameters needed for a single rotation (motor phases, LED timing, etc)
void calculate_melty_params(spin_control_parameters_t* params, ctrl_state* c) {
    float rpm = robot.get_rpm();
    pid_current_rpm = rpm;
    long rotation_us = (1.0f/rpm) * 60 * 1000 * 1000;

    // because by default we're spinning clockwise, right turns = extending the rotation interval
    // Fortunately for us, on the xbox, stick right = positive values, while stick left = negative
    float rpm_adjustment_factor = c->turn_lr / LEFT_RIGHT_HEADING_CONTROL_DIVISOR;

    rotation_us += (long) rotation_us*rpm_adjustment_factor/LEFT_RIGHT_HEADING_CONTROL_DIVISOR;

    // if we're too slow, don't even try to track heading
    if (rotation_us > MAX_TRACKING_ROTATION_INTERVAL_US) {
        rotation_us = MAX_TRACKING_ROTATION_INTERVAL_US;
    }

    params->rotation_interval_us = rotation_us;

    // and the LED settings
    float led_on_portion = rpm / MAX_TRACKING_RPM;
    if (led_on_portion < 0.10f) led_on_portion = 0.10f;
    if (led_on_portion > 0.90f) led_on_portion = 0.90f;

    unsigned long led_on_us = led_on_portion * rotation_us;
    unsigned long led_offset_us = LED_OFFSET_PERCENT * rotation_us;

    // starts LED on time at point in rotation so it's "centered" on led offset
    params->led_start = led_offset_us - (led_on_us / 2);
    if (params->led_start < 0) {
        params->led_start += rotation_us;
    }
    
    params->led_stop = params->led_start + led_on_us;
    if (params->led_stop > rotation_us)
    {
        params->led_stop -= rotation_us;
    }

     // phase transition timing: Currently, only forwards/backwards, so we start the phases at 0 and 1/2 a rotation
    params->motor_start_phase_1 = 0;
    params->motor_start_phase_2 = rotation_us / 2;

    // The throttle PID control
    pid_target_rpm = c->target_rpm;
    throttle_pid.Compute();
    params->throttle_perk = (int) pid_throttle_output;

    // todo - translation trim instead of just *1
    params->max_throttle_offset = c->translate_forback * params->throttle_perk * 1 / 1024;
}

// Arduino loop function. Runs in CPU 1.
void loop() {
    BP32.update();

    ctrl_state* c = ctrl_update();

    if (!c->connected) {
        throttle_pid.SetMode(MANUAL);
        state = NO_CONTROLLER;
    } else if (!c->alive) {
        throttle_pid.SetMode(MANUAL);
        state = CONTROLLER_STALE;
    } else if (c->spin_requested) {
        if (state != SPINNING) {
            // we're just starting to spin. Start the PID
            throttle_pid.SetMode(AUTOMATIC);
        }

        state = SPINNING;
        calculate_melty_params(&control_params, c);
    } else {
        throttle_pid.SetMode(MANUAL);
        state = READY;

        tank_params.translate_forback = c->translate_forback;
        tank_params.turn_lr = c->turn_lr;
    }

    if (millis() - last_logged_at > 500) {
        Serial.printf("Controller state: connected: %d alive: %d spin: %d vThrottle: %d \n", c->connected, c->alive, c->spin_requested, c->target_rpm);
        last_logged_at = millis();
    }

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    vTaskDelay(10);
}

// The robot control loop. Runs in CPU 0.
void hotloopFN(void* parameter) {
    while(true) {
        // do the magic stuff
        robot.update_loop(state, &control_params, &tank_params);

        //TODO: Update this to a proper, configurable delay - probably 1-4khz
        vTaskDelay(1);
    }
}