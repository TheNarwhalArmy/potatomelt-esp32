#include "led.h"
#include "battery.h"

// The main struct shared by the robot side and the control side threads - contains the state of what we want the robot to do
typedef struct control_parameters_t {
    bool spin_enabled;                  // Authorization for the robot to spin
    int translation_enabled;            // Authorization for the spinning bot to translate
    int throttle_percent;               // stores throttle
    int max_throttle_offset;            // In a rotation, the furthest from the base throttle setting that each motor should be spun
    unsigned long rotation_interval_us; // time for 1 rotation of robot
    unsigned long led_start;            // offset for beginning of LED beacon
    unsigned long led_stop;             // offset for end of LED beacon
    unsigned long motor_start_phase_1;  // time offset for when motor 1 begins translating forwards
    unsigned long motor_start_phase_2;  // time offset for when motor 2 begins translating forwards
    int battery_percent;                // battery power remaining- where on the green->red slope we should be
};

// And the parent Robot class
class Robot {
    public:
        void init();
        void update_loop(control_parameters_t* params);
    private:
        unsigned long rotation_started_at_us;
        LED leds;
        Battery battery;

        // temp values for testing components
        int led_status_temp;
};