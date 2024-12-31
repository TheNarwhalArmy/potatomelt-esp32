#include <Arduino.h>

class DShot {
    public:
        DShot();
        void send_packet(uint16_t throttle);
        void init(int pin);
    private:
        uint16_t create_packet(uint16_t throttle);
        int dshot_pin;
        rmt_data_t* rmt_data;
};