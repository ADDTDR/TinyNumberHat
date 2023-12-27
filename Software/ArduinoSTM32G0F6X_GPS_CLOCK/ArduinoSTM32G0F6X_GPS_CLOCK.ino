

#include <Wire.h>


#define HT16K33_ADDRESS_0          0x70
#define HT16K33_TURN_ON_OSCILLATOR 0x21
#define HT16K33_ENABLE_DISPLAY     0x81
#define HT16K33_CMD_BRIGHTNESS     0xE0
#define LED_DRIVER_BRIGHTNESS_LEVEL 0x0F

class TinyNumberHat {
public:
  uint8_t const font7Seg[16] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  uint8_t buffer[16];
  
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

    // Clear display
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(0x00);
    for (int i = 0; i < 16; i++) {
      Wire.write(0x00);
    }
    Wire.endTransmission();
  }

  void numericTest(){
      Wire.beginTransmission(ht16k33I2CAddress);
      Wire.write(0x00);
      for(int i = 0; i < 16; i ++){
        Wire.write(font7Seg[i]);
      }
      Wire.endTransmission();
  }

  void fullTurnOnTest(){
    // Fill with 1, turn on all segments
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(0x00);
    for (int i = 0; i < 16; i++) {
      Wire.write(0xFF);
    }
    Wire.endTransmission();
  }

  void update(){
      Wire.beginTransmission(ht16k33I2CAddress);
      Wire.write(0x00);
      for(int i = 0; i < 16; i ++){
        Wire.write(buffer[i]);
      }
      Wire.endTransmission();
  }

  void clear(){
    Wire.beginTransmission(ht16k33I2CAddress);
    Wire.write(0x00);
    for(int i = 0; i < 16; i ++){
      Wire.write(0x00);
    }
    Wire.endTransmission();
  }

  
private:
  uint8_t ht16k33I2CAddress;
};


String extractTimeFromNMEA(String sentence) {
  // Check if the sentence is a GNRMC sentence
  if (sentence.substring(0, 6) == "$GNRMC") {
    //Copy time substring from pos 7 to 13
    return sentence.substring(7, 15);
  } else {
    return "000000.0";
  }
}

TinyNumberHat tinyNumberHat;


void setup()
{
  tinyNumberHat.begin();
  tinyNumberHat.fullTurnOnTest();
  delay(200);
  Serial.begin(9600);
}

String time_str;
int gmt_offset = 2;
void loop()
{
     if (Serial.available() > 0) {
    // Read a line until a newline character is encountered
      String receivedData = Serial.readStringUntil('\n');
      if (receivedData.substring(0, 6) == "$GNRMC"){
        time_str = extractTimeFromNMEA(receivedData); 
        int hour_gmt = time_str.substring(0, 2).toInt(); 
        hour_gmt = (hour_gmt + gmt_offset) % 24; 
        tinyNumberHat.buffer[0] = tinyNumberHat.font7Seg[hour_gmt / 10];
        tinyNumberHat.buffer[1] = tinyNumberHat.font7Seg[hour_gmt % 10];
        tinyNumberHat.buffer[2] = 0b01000000; // Add '-'
        tinyNumberHat.buffer[3] = tinyNumberHat.font7Seg[time_str[2] - '0'];
        tinyNumberHat.buffer[4] = tinyNumberHat.font7Seg[time_str[3] - '0'];
        tinyNumberHat.buffer[5] = 0b01000000; // Add '-'
        tinyNumberHat.buffer[6] = tinyNumberHat.font7Seg[time_str[4] - '0'];
        tinyNumberHat.buffer[7] = tinyNumberHat.font7Seg[time_str[5] - '0'];
        tinyNumberHat.buffer[7] = tinyNumberHat.buffer[7] | 0b10000000; // Add '.' to ds8 
        tinyNumberHat.buffer[8] = tinyNumberHat.font7Seg[time_str[7] - '0' ];
        tinyNumberHat.update();
     }
    
    }
 

}


