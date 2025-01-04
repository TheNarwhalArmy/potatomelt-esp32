class IMU {
    public:
        IMU();
        float get_rpm();
        bool get_inverted();
        void poll();
        void init();
        float z_accel_buffer = 0.0;
    private:
        void set_z_offset();
        
};