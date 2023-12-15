#include <Wire.h>

#define HT16K33_ADDRESS_0          0x70
#define HT16K33_TURN_ON_OSCILLATOR 0x21
#define HT16K33_ENABLE_DISPLAY     0x81
#define HT16K33_CMD_BRIGHTNESS     0xE0
#define LED_DRIVER_BRIGHTNESS_LEVEL 0x0F

class TinyNumberHat {
public:
  void begin() {
    Wire.begin(); // Initialize I2C
    ht16k33I2CAddress = HT16K33_ADDRESS_0;

    // Turn on oscillator
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(HT16K33_TURN_ON_OSCILLATOR);
    Wire.endTransmission();

    // Enable display (no blinking mode)
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(HT16K33_ENABLE_DISPLAY);
    Wire.endTransmission();

    // Clear display
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(0x00);
    for (int i = 0; i < 16; i++) {
      Wire.write(0x00);
    }
    Wire.endTransmission();

    // Set brightness 0-15
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(HT16K33_CMD_BRIGHTNESS | LED_DRIVER_BRIGHTNESS_LEVEL);
    Wire.endTransmission();

    // Fill with 1, turn on all segments
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(0x00);
    for (int i = 0; i < 16; i++) {
      Wire.write(0xFF);
    }
    Wire.endTransmission();

    delay(5100); // Sleep for 5.1 seconds

    // Clear display
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(0x00);
    for (int i = 0; i < 16; i++) {
      Wire.write(0x00);
    }
    Wire.endTransmission();
  }

private:
  uint8_t ht16k33I2CAddress;
};

TinyNumberHat tinyNumberHat;

void setup() {
  tinyNumberHat.begin();
}

void loop() {
  // Your loop code here
}
