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

int i = 0;

// Arduino loop function. Runs in CPU 1.
void loop() {
    BP32.update();

    ctrl_state* c = ctrl_update();

    if (!c->connected) {
        state = NO_CONTROLLER;
    } else if (!c->alive) {
        state = CONTROLLER_STALE;
    } else {
        state = READY;

        // todo - move tank-params updating elsewhere?
        tank_params.translate_forback = c->translate_forback;
        tank_params.turn_lr = c->turn_lr;
    } // todo - compute melty parameters (!)

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