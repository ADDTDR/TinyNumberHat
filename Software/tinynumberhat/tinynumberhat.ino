#include <Wire.h>
#include <unordered_map>

#define HT16K33_ADDRESS_0          0x70
#define HT16K33_TURN_ON_OSCILLATOR 0x21
#define HT16K33_ENABLE_DISPLAY     0x81
#define HT16K33_CMD_BRIGHTNESS     0xE0
#define LED_DRIVER_BRIGHTNESS_LEVEL 0x0F

class TinyNumberHat {
public:
   std::unordered_map<char, uint8_t> numbers = {
        {'0', 0b00111111},
        {'1', 0b00000110},
        {'2', 0b01011011},
        {'3', 0b01001111},
        {'4', 0b01100110},
        {'5', 0b01101101},
        {'6', 0b01111101},
        {'7', 0b00000111},
        {'8', 0b01111111},
        {'9', 0b01101111},
        {'.', 0b10000000},
        {'P', 0b01110011},
        {'L', 0b00111000},
        {'A', 0b01110111},
        {'Y', 0b01101110},
        {'S', 0b01101101},
        {'T', 0b01111000},
        {'O', 0b01011100},
        {'U', 0b00111110},
        {'E', 0b01111001},
        {' ', 0b00000000},
        {'H', 0b01110110},
        {'-', 0b01000000}
    };

  uint8_t charToBin(char ch){
    auto it = numbers.find(ch);
    if (it != numbers.end()) {
        return it->second;
    } else {
        // Default to 0 for unknown characters
        return 0;
    }
  }
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
