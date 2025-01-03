#include <Preferences.h>

class Storage{
    public:
        void init();
        int get_target_rpm();
        void set_target_rpm(int tar);
    private:
        Preferences preferences;
};

Storage get_active_store();