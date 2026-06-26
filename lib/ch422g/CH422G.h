#pragma once

#include <Arduino.h>
#include <Wire.h>

// CH422G I/O expander — I2C addresses encode the command (not the register).
// 0x30 = write OE (output-enable) register
// 0x38 = write output value register
// On the Waveshare ESP32-S3-Touch-LCD-4.3:
//   EXIO1 (bit 1) = GT911 touch reset
//   EXIO2 (bit 2) = display backlight enable

#define CH422G_ADDR_OE  0x30
#define CH422G_ADDR_OUT 0x38

#define CH422G_EXIO1_BIT  (1 << 1)  // Touch RST
#define CH422G_EXIO2_BIT  (1 << 2)  // Backlight enable

class CH422G {
 public:
  void begin(int sda, int scl);
  void setOutput(uint8_t bits);
  void setPin(uint8_t bit, bool high);

 private:
  uint8_t _output = 0;
};

extern CH422G ch422g;
