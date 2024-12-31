#include <Wire.h>
#include "src/robot.h"
#include "melty_config.h"

TaskHandle_t hotloop;
long lastUpdated = 0;

Robot robot;

control_parameters_t control_params;

// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);
    Serial.println("PotatoMelt startup");

    // set up I2C
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    robot.init();

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
    // todo: Get robot status in a more compartmentalized way
    Serial.printf("Z accel: %f, keepalive: %d\n", robot.get_z_buffer(), i);
    i++;

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

        //TODO: Update this to a proper, configurable delay - probably 1-4khz
        // Probably 1-4 khz
        vTaskDelay(15);
    }
}