#include <Preferences.h>

class Storage{
    public:
        void init();
        int get_target_rpm();
        void set_target_rpm(int rpm);
        float get_accel_correction(int rpm);
        void set_accel_correction(int rpm, float corr);
    private:
        Preferences preferences;
};

Storage* get_active_store();