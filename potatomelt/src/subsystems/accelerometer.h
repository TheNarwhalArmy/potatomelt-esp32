#include "src/lib/SparkFun_LIS331.h"

class Accelerometer {
    public:
        Accelerometer();
        void init(int addr);
        void sample_offset();
        float get_z_accel();
    private:
        LIS331 lis;
        int sample_count;
        float summed_samples;
        float z_offset;
};