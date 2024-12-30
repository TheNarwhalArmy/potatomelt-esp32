#include "robot.h"

TaskHandle_t hotloop;
long lastUpdated = 0;

Robot robot;
Battery battery;

control_parameters_t control_params;

// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);
    Serial.println("PotatoMelt startup");

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

    robot.init();
}

// Arduino loop function. Runs in CPU 1.
void loop() {
    // todo: Get robot status in a more compartmentalized way
    Serial.printf("Battery voltage: %f\n", battery.get_voltage());

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    vTaskDelay(500);
}

// The robot control loop. Runs in CPU 0.
void hotloopFN(void* parameter) {
    while(true) {
        // do the magic stuff
        robot.update_loop(&control_params);

        //TODO: Update this to an appropriate, configurable delay
        // Probably 1-4 khz
        vTaskDelay(15);
    }
}