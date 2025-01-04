#include "../lib/SparkFun_LIS331_ESP32.h"

class Accelerometer {
    public:
        Accelerometer();
        void init(int addr);
        void sample_offset();
        float get_z_accel();
        float get_xy_accel();
    private:
        LIS331ESP lis;
        int sample_count;
        float summed_samples;
        float z_offset;
};