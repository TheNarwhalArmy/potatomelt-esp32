#include "storage.h"

Storage active;

Storage get_active_store() {
    return active;
}

void Storage::init() {
    preferences.begin("potatomelt", false);
}

int Storage::get_target_rpm() {
    return preferences.getInt("target_rpm_index", 3);
}


void Storage::set_target_rpm(int tar) {
    preferences.putInt("target_rpm_index", tar);
}