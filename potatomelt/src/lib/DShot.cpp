#include "DShot.h"

#define DSHOT_ESC_RESOLUTION_HZ 10000000 // 10mhz resolution: 1 tick == 100 nanoseconds

DShot::DShot() {
    rmt_data = new rmt_data_t[17];
}

void DShot::init(int pin) {
    dshot_pin = pin;
    rmtInit(dshot_pin, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, DSHOT_ESC_RESOLUTION_HZ);
}

uint16_t DShot::create_packet(uint16_t throttle) {
  uint8_t csum = 0;
  throttle <<= 1;
	// Indicate as command if less than 48
	if (throttle < 48 && throttle > 0)
		throttle |= 1;
  uint16_t csum_data = throttle;
  for (byte i=0; i<3; i++) {
    csum ^= csum_data;
    csum_data >>= 4;
  }
  csum &= 0xf;
  return (throttle<<4)|csum;
}

void DShot::send_packet(uint16_t throttle) {
  uint16_t packet = create_packet(throttle);

  for(int i = 0; i < 16; i++) {
    if ((packet >> i) & 1 ) {
        rmt_data[i].level0 = 1;
        rmt_data[i].duration0 = 25;
        rmt_data[i].level1 = 0;
        rmt_data[i].duration1 = 8;
      } else {
        rmt_data[i].level0 = 1;
        rmt_data[i].duration0 = 12;
        rmt_data[i].level1 = 0;
        rmt_data[i].duration1 = 21;
      }
    }

    rmt_data[17].level0 = 1;
    rmt_data[17].level1 = 0;

    rmt_data[17].duration1 = 50;

    rmtWriteLooping(dshot_pin, rmt_data, 16);
  }

