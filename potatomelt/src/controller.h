#include <Bluepad32.h>

typedef struct ctrl_state {
    // the controller failsafes - we need to be both connected and alive for the robot to do anything other than blink LEDs at us
    bool connected;
    bool alive;

    // the master power switch - if this is false, we are not permitted to spin
    bool spin_requested;

    // translation commands and spin direction
    int translate_forback; // -512 - 512
    int translate_lr; // reserved for future implementation
    int turn_lr; // -512 - 512
    bool reverse_spin; // toggle, stays true between presses of the command button
    int target_rpm; // this may stay high even if spin_requested is false!

    float translate_trim;

    // and parameter tweaking
    // all of these are edge detectors, they'll go true once when the button is pressed and then drop back to false
    bool trim_left;
    bool trim_right;
};

typedef struct prev_state {
    // for keeping track of what's changed since last update
    bool reverse_spin_pressed;
    bool increase_translate_pressed;
    bool decrease_translate_pressed;
    bool trim_left_pressed;
    bool trim_right_pressed;
    bool spin_target_rpm_changed;
    long last_trim_at;
};

void ctrl_init();

// connect and disconnect callbacks for bpad32
void on_connected_controller(ControllerPtr ctr);
void on_disconnected_controller(ControllerPtr ctr);

// and what we all came here for, the controller interface functions
// update() should be called regardless of if there's been a controller update or not
bool is_connected();
ctrl_state* ctrl_update(bool upd8);
ctrl_state* get_state(ControllerPtr ctl);

// Vibrate the controller based on robot RPM
// This function makes the controller vibrate with intensity proportional to how fast 
// the entire robot (not the motors) is spinning. Uses actual RPM from IMU sensors.
// 
// RPM range: 400 (MIN_TRACKING_RPM) to 3000 RPM
// Vibration intensity: 0% at 400 RPM, 100% at 3000 RPM
// Update frequency: Limited to every 200ms to avoid overloading controller
//
// Note: If compilation fails due to unknown setRumble method, check the Bluepad32 
// documentation for your version and update the API call in controller.cpp
void ctrl_vibrate_for_rpm(float actual_rpm);

// Test function to validate vibration intensity calculations
// Call this during setup() to verify the RPM-to-vibration mapping works correctly
void ctrl_test_vibration_logic();
