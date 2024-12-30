#include <Arduino.h>
#include "battery.h"
#include "melty_config.h"

float Battery::get_voltage() {
    uint32_t adc_reading = analogReadMilliVolts(BATTERY_ADC_PIN);
    return adc_reading * BATTERY_VOLTAGE_DIVIDER / 1000;
}

int Battery::get_percent() {
    float battery_cell_volts = get_voltage() / BATTERY_CELL_COUNT;
    int battery_percent = (int) (battery_cell_volts - BATTERY_CELL_EMPTY_VOLTAGE) / (BATTERY_CELL_FULL_VOLTAGE - BATTERY_CELL_EMPTY_VOLTAGE);
    return battery_percent;
}