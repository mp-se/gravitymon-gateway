#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

static const uint8_t LED_BUILTIN = 43; // No GPIO enabled LED on this board, setting TX to avoid conflicts since this will not be used
#define BUILTIN_LED  LED_BUILTIN // backward compatibility
#define LED_BUILTIN LED_BUILTIN  // allow testing #ifdef LED_BUILTIN

static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t SDA = 11;
static const uint8_t SCL = 10;

static const uint8_t A15 = 15;
static const uint8_t A18 = 18;
static const uint8_t A19 = 19;
static const uint8_t A20 = 20;

static const uint8_t SS    = 21;
static const uint8_t MOSI  = 17;
static const uint8_t MISO  = 16;
static const uint8_t SCK   = 14;

#endif /* Pins_Arduino_h */
