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
        {'O', 0b00111111}, // Corrected to 0b00111111
        {'U', 0b00111110},
        {'E', 0b01111001},
        {' ', 0b00000000},
        {'H', 0b01110110},
        {'-', 0b01000000},
        {'B', 0b01111100},
        {'C', 0b00111001},
        {'D', 0b01011110},
        {'F', 0b01110001},
        {'G', 0b01111101},
        {'I', 0b00000110},
        {'J', 0b00011110},
        {'K', 0b01110101}, // Representation for K
        {'M', 0b01010100}, // Representation for M
        {'N', 0b01010100}, // Representation for N
        {'Q', 0b01101011}, // Representation for Q
        {'R', 0b01010000}, // Representation for R
        {'V', 0b00111110},
        {'W', 0b00101010}, // Representation for W
        {'X', 0b01110110}, // Same as H
        {'Z', 0b01011011}, // Same as 2
        {'!', 0b10000110}, // Representation for !
        {'?', 0b01010011}, // Representation for ?
        {'*', 0b01100011}, // Representation for *
        {'#', 0b01111100}, // Representation for #
        {'$', 0b01101101}  // Representation for $
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

    delay(100); // Sleep for 5.1 seconds

    // Clear display
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(0x00);
    for (int i = 0; i < 16; i++) {
      Wire.write(0x00);
    }
    Wire.endTransmission();
  }

  void displayString(const char* str) {
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(0x00);
    for (int i = 0; i < 9; i++) {
      if (str[i] != '\0') {
        Wire.write(charToBin(str[i]));
      } else {
        Wire.write(0x00); // Write blank if string is shorter than 9 chars
      }
      // Wire.write(0x00); // Space between characters
    }
    Wire.endTransmission();
  }


    void displayDots() {
        for (int i = 0; i < 16; i++) {
            Wire.beginTransmission(ht16k33I2CAddress);
            Wire.write(0x00);
            for (int j = 0; j < 16; j++) {
                Wire.write((j == i) ? 0x80 : 0x00);
            }
            Wire.endTransmission();
            delay(100); // Delay between each dot
        }
    }

   void snakeAnimation() {
        uint8_t snake[16] = {0};
        for (int i = 0; i < 16; i++) {
            snake[i] = 0x01 << i;
            Wire.beginTransmission(ht16k33I2CAddress);
            Wire.write(0x00);
            for (int j = 0; j < 16; j++) {
                Wire.write(snake[j]);
            }
            Wire.endTransmission();
            delay(100); // Delay between each step of the snake
            snake[i] = 0x00;
        }
    }

private:
  uint8_t ht16k33I2CAddress;
};

TinyNumberHat tinyNumberHat;

void setup() {
  tinyNumberHat.begin();
}

void loop() {
    const char* sequences[] = {
        "HELLO", "WORLD", "123456789", "PLAY", "GOODBYE", 
        "A1B2C3", "DOG", "CAT", "QWERTY", "SNAKE", 
        "JUMP", "KICK", "FROG", "ZEBRA", "NIGHT",
        "LIGHT", "HAPPY", "FUNNY", "F!G#H$"
    };
    for (int i = 0; i < sizeof(sequences)/sizeof(sequences[0]); i++) {
        tinyNumberHat.displayString(sequences[i]);
        delay(500); // Display each sequence for 0.5 seconds
    }
  tinyNumberHat.displayDots();
  tinyNumberHat.snakeAnimation();
}
