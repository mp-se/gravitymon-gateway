#include "CH422G.h"

CH422G ch422g;

void CH422G::begin(int sda, int scl) {
  Wire.begin(sda, scl, 400000);

  // Enable all EXIO pins as outputs
  Wire.beginTransmission(CH422G_ADDR_OE);
  Wire.write(0xFF);
  Wire.endTransmission();

  // Start with all outputs low
  _output = 0;
  Wire.beginTransmission(CH422G_ADDR_OUT);
  Wire.write(_output);
  Wire.endTransmission();
}

void CH422G::setOutput(uint8_t bits) {
  _output = bits;
  Wire.beginTransmission(CH422G_ADDR_OUT);
  Wire.write(_output);
  Wire.endTransmission();
}

void CH422G::setPin(uint8_t bit, bool high) {
  if (high)
    _output |= bit;
  else
    _output &= ~bit;
  Wire.beginTransmission(CH422G_ADDR_OUT);
  Wire.write(_output);
  Wire.endTransmission();
}
