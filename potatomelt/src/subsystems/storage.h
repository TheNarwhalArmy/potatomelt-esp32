#include <Preferences.h>

class Storage{
    public:
        void init();
        int get_target_rpm();
        void set_target_rpm(int tar);
        float get_accel_correction();
        void set_accel_correction(float corr);
    private:
        Preferences preferences;
};

Storage get_active_store();