#include <Arduino.h>
#include <driver/rmt.h>

// This file has all the hard-coded settings for Potatomelt

// ------------ Pin and RMT Mappings -----------------

#define NEOPIXEL_PIN GPIO_NUM_17
#define MOTOR_1_PIN GPIO_NUM_6
#define MOTOR_2_PIN GPIO_NUM_7

#define NEOPIXEL_RMT RMT_CHANNEL_0
#define MOTOR_1_RMT RMT_CHANNEL_1
#define MOTOR_2_RMT RMT_CHANNEL_2

#define I2C_SDA_PIN 8
#define I2C_SCL_PIN 9
#define BATTERY_ADC_PIN 10

// ------------ Battery Configuration ---------------

#define BATTERY_ALERT_ENABLED                     // if enabled - heading LED will flicker when battery voltage is low
#define BATTERY_CRIT_HALT_ENABLED                 // if enabled - robot will halt when battery voltage is critically low
#define BATTERY_VOLTAGE_DIVIDER 8.24              // From the PCB - what's the voltage divider betweeen the battery + and the sense line?
#define BATTERY_CELL_COUNT 3                      // How many cells are in the battery?
#define BATTERY_CELL_FULL_VOLTAGE 4.2             // What voltage is a fully-charged cell? Standard lipos are 4.2v, other chemistries will vary
#define BATTERY_CELL_EMPTY_VOLTAGE 3.2            // And on the other hand, what voltage is an empty cell? We're going to cut off at 3.2v/cell
#define LOW_BAT_REPEAT_READS_BEFORE_ALARM 20      // Requires this many ADC reads below threshold before halting the robot