enum Status {
    TANK,
    LOS,
    BATTERY
};

class LED {
    public:
        LED();
        void leds_on(Status status);
        void leds_on_gradient(int color);
        void leds_off();
    private:
        void leds_on_rgb(int red, int green, int blue);
        void write_pixel();
};