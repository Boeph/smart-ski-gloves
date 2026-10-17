#pragma once
#include <HID.h>
#include <Keyboard.h>

#define CONSUMER_PLAY_PAUSE   0x00CD
#define CONSUMER_NEXT         0x00B5
#define CONSUMER_PREVIOUS     0x00B6
#define CONSUMER_VOL_UP       0x00E9
#define CONSUMER_VOL_DOWN     0x00EA

static const uint8_t _consumerDescriptor[] PROGMEM = {
  // Consumer Control
  0x05, 0x0C,        // Usage Page (Consumer)
  0x09, 0x01,        // Usage (Consumer Control)
  0xA1, 0x01,        // Collection (Application)
  0x85, 0x03,        //   Report ID (3)
  0x19, 0x00,        //   Usage Minimum (0)
  0x2A, 0xFF, 0x02,  //   Usage Maximum (0x2FF)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x02,  //   Logical Maximum (0x2FF)
  0x75, 0x10,        //   Report Size (16)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x00,        //   Input (Data, Array)
  0xC0               // End Collection
};

class ConsumerControl_ : public HIDSubDescriptor {
public:
  ConsumerControl_() : HIDSubDescriptor(_consumerDescriptor, sizeof(_consumerDescriptor)) {
    // removed from constructor intentionally
  }

  void begin() {
    HID().AppendDescriptor(this);  // register AFTER Keyboard.begin()
  }

  void write(uint16_t key) {
    uint8_t report[2] = { (uint8_t)(key & 0xFF), (uint8_t)(key >> 8) };
    HID().SendReport(3, report, 2);
    delay(10);
    uint8_t empty[2] = {0, 0};
    HID().SendReport(3, empty, 2);
  }
};

ConsumerControl_ ConsumerControl;