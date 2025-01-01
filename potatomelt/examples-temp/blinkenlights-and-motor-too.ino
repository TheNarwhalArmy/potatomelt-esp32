// included here for reference
// this will get deleted later

#include <Bluepad32.h>
#include <Arduino.h>
#include <driver/rmt.h>
#include "DShotRMT.h"

#define NEO_PIN GPIO_NUM_17
#define NEO_RMT RMT_CHANNEL_2
#define MOTOR_PIN GPIO_NUM_6
#define MOTOR_RMT RMT_CHANNEL_1

const auto DSHOT_MODE = DSHOT300;
const auto FAILSAFE_THROTTLE = 0;
const auto INITIAL_THROTTLE = 0;

DShotRMT motor(MOTOR_PIN, MOTOR_RMT);

int post = 0;
int ctr = 0;
int throttle = 0;

bool is_x = false;
bool is_controller = false;

rmt_item32_t led_data[6*8];
uint8_t pixel_color[6]; 

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void advance_colors() {
    post++;
    if (post == 256) {
      post -= 256;
    }

    int pos = post;
    if (pos < 85) {
      setPixelColor(0, 255-pos*3, pos*3, 0);
      setPixelColor(1, 0, 255-pos*3, pos*3);
    } else if (pos < 170) {
      pos -= 85;
      setPixelColor(0, 0, 255-pos*3, pos*3);
      setPixelColor(1, pos*3, 0, 255-pos*3);
    } else {
      pos -= 170;
      setPixelColor(0, pos*3, 0, 255-pos*3);
      setPixelColor(1, 255-pos*3, pos*3, 0);
    }

    writePixel();
}

void LEDs_off() {
    setPixelColor(0, 0, 0, 0);
    setPixelColor(1, 0, 0, 0);

    writePixel();
}

void LEDs_red() {
    setPixelColor(0, 255, 0, 0);
    setPixelColor(1, 255, 0, 0);

    writePixel();
}


void setPixelColor(int pix, int r, int g, int b) {
    int index = pix * 3;
    pixel_color[index] = g;
    pixel_color[index+1] = r;
    pixel_color[index+2] = b;
}

void writePixel() {
    int index = 0;
    for (auto chan : pixel_color) {
      uint8_t value = chan;
      for (int bit = 7; bit >= 0; bit--) {
        if ((value >> bit) & 1) {
          led_data[index].level0 = 1;
          led_data[index].duration0 = 8;
          led_data[index].level1 = 0;
          led_data[index].duration1 = 4;
        } else {
          led_data[index].level0 = 1;
          led_data[index].duration0 = 4;
          led_data[index].level1 = 0;
          led_data[index].duration1 = 8;
        }
        index++;
      }
    }

    rmt_write_items(NEO_RMT, led_data, 6*8, false);
}

void motor_stop() {
    throttle = 0;
}

void motor_go(int t) {
    if (t > 0) t += 48;

    // cap it to 1047, because escs spin both ways
    t = min(t, 1047);

    throttle = t;
}


// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    is_controller = true;

    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }

    // Anguirel says: There is an issue where scan_evt will timeout eventually causing something to print "FEX x y",
    // (where x and y are various numbers) to the console and then eventually crash. Possible occurence of
    // https://github.com/ricardoquesada/bluepad32/issues/43.  The reported workaround is to disable scanning
    // for new controllers once a controller has connected.
    BL32.enableNewBluetoothConnections(false);
}

void onDisconnectedController(ControllerPtr ctl) {
    is_controller = false;

    bool foundController = false;

    motor_stop();

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }

    BL32.enableNewBluetoothConnections(true);
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
}

void processGamepad(ControllerPtr ctl) {
    is_x = ctl->buttons() & 0x0004;
    
    if (ctl->brake() > 0) {
        motor_go(ctl->brake());
    } else {
        motor_stop();
    }


    // Another way to query controller data is by getting the buttons() function.
    // See how the different "dump*" functions dump the Controller info.

    if (ctr % 10 == 0)
        dumpGamepad(ctl);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

// Arduino setup function. Runs in CPU 1
void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // set up the RMT
    Serial.printf("Configuring RMT\n");

    rmt_config_t rmt_cfg = RMT_DEFAULT_CONFIG_TX(NEO_PIN, NEO_RMT);

    rmt_cfg.clk_div = 8; // theoretically, slow us down to 10mhz

    rmt_config(&rmt_cfg);

    rmt_driver_install(rmt_cfg.channel, 0, 0);

    LEDs_red();

    // set up the motor

    Serial.printf("Configuring dshot\n");

    motor.begin(DSHOT_MODE);

    motor_stop();

    Serial.printf("Configuring BP32\n");

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // Enables mouse / touchpad support for gamepads that support them.
    // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
    // - First one: the gamepad
    // - Second one, which is a "virtual device", is a mouse.
    // By default, it is disabled.
    BP32.enableVirtualDevice(false);
}

// Arduino loop function. Runs in CPU 1.
void loop() {
    // This call fetches all the controllers' data.
    // Call this function in your main loop.
    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();

    if (is_controller) {
        if (is_x) {
            advance_colors();
        } else {
            LEDs_off();
        }
    } else {
        LEDs_red();
    }

    ctr++;

    motor.sendThrottleValue(throttle);    

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    //     vTaskDelay(1);
    delay(10);
}
