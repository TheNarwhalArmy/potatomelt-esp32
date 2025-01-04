class IMU {
    public:
        IMU();
        float get_rpm();
        bool get_inverted();
        void poll();
        void init();
        void trim(bool increase);
        float get_accel_1_g();
        float get_accel_2_g();
        float z_accel_buffer = 0.0;
    private:
        void set_z_offset();
        
};