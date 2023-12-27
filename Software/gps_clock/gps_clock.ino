
/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <ArduinoNmeaParser.h>
#include <Wire.h>



#define HT16K33_ADDRESS_0          0x70
#define HT16K33_TURN_ON_OSCILLATOR 0x21
#define HT16K33_ENABLE_DISPLAY     0x81
#define HT16K33_CMD_BRIGHTNESS     0xE0
#define LED_DRIVER_BRIGHTNESS_LEVEL 0x0F

class TinyNumberHat {
public:
  uint8_t const test_data[16] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
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

  void numericTest(){
      Wire.beginTransmission(ht16k33I2CAddress);
      Wire.write(0x00);
      for(int i = 0; i < 16; i ++){
        Wire.write(test_data[i]);
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
private:
  uint8_t ht16k33I2CAddress;
};


/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

void onRmcUpdate(nmea::RmcData const);
void onGgaUpdate(nmea::GgaData const);

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

ArduinoNmeaParser parser(onRmcUpdate, onGgaUpdate);

TinyNumberHat tinyNumberHat;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{

  tinyNumberHat.begin();
  tinyNumberHat.numericTest();
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop()
{
  while (Serial1.available()) {
    parser.encode((char)Serial1.read());
  }
}

/**************************************************************************************
 * FUNCTION DEFINITION
 **************************************************************************************/

void onRmcUpdate(nmea::RmcData const rmc)
{
  Serial.print("RMC ");

  if      (rmc.source == nmea::RmcSource::GPS)     Serial.print("GPS");
  else if (rmc.source == nmea::RmcSource::GLONASS) Serial.print("GLONASS");
  else if (rmc.source == nmea::RmcSource::Galileo) Serial.print("Galileo");
  else if (rmc.source == nmea::RmcSource::GNSS)    Serial.print("GNSS");
  else if (rmc.source == nmea::RmcSource::BDS)     Serial.print("BDS");

  Serial.print(" ");
  Serial.print(rmc.time_utc.hour);
  int time_gmt = (rmc.time_utc.hour + 2) % 24;
  int tens = time_gmt / 10;      // tens now = 2
  int ones = time_gmt % 10;      // ones now = 6
  tinyNumberHat.buffer[0] = tinyNumberHat.test_data[tens];
  tinyNumberHat.buffer[1] = tinyNumberHat.test_data[ones];
  tinyNumberHat.buffer[2] = 0b01000000; // Add '-'
  

  Serial.print(":");
  Serial.print(rmc.time_utc.minute);
  tinyNumberHat.buffer[3] = tinyNumberHat.test_data[rmc.time_utc.minute / 10]; // tens 
  tinyNumberHat.buffer[4] = tinyNumberHat.test_data[rmc.time_utc.minute % 10]; // ones 
  tinyNumberHat.buffer[5] = 0b01000000; // Add '-'

  Serial.print(":");
  Serial.print(rmc.time_utc.second);
  tinyNumberHat.buffer[6] = tinyNumberHat.test_data[rmc.time_utc.second / 10];
  tinyNumberHat.buffer[7] = tinyNumberHat.test_data[rmc.time_utc.second % 10];
  tinyNumberHat.buffer[7] = tinyNumberHat.buffer[7] | 0b10000000; //  Add '.'
  

  Serial.print(".");
  Serial.print(rmc.time_utc.microsecond);
  tinyNumberHat.buffer[8] = tinyNumberHat.test_data[rmc.time_utc.microsecond];
  tinyNumberHat.update();

  if (rmc.is_valid)
  {
    Serial.print(" : LON ");
    Serial.print(rmc.longitude);
    Serial.print(" ° | LAT ");
    Serial.print(rmc.latitude);
    Serial.print(" ° | VEL ");
    Serial.print(rmc.speed);
    Serial.print(" m/s | HEADING ");
    Serial.print(rmc.course);
    Serial.print(" °");
  }

  Serial.println();
}

void onGgaUpdate(nmea::GgaData const gga)
{
  Serial.print("GGA ");

  if      (gga.source == nmea::GgaSource::GPS)     Serial.print("GPS");
  else if (gga.source == nmea::GgaSource::GLONASS) Serial.print("GLONASS");
  else if (gga.source == nmea::GgaSource::Galileo) Serial.print("Galileo");
  else if (gga.source == nmea::GgaSource::GNSS)    Serial.print("GNSS");
  else if (gga.source == nmea::GgaSource::BDS)     Serial.print("BDS");

  Serial.print(" ");
  Serial.print(gga.time_utc.hour);
  Serial.print(":");
  Serial.print(gga.time_utc.minute);
  Serial.print(":");
  Serial.print(gga.time_utc.second);
  Serial.print(".");
  Serial.print(gga.time_utc.microsecond);

  if (gga.fix_quality != nmea::FixQuality::Invalid)
  {
    Serial.print(" : LON ");
    Serial.print(gga.longitude);
    Serial.print(" ° | LAT ");
    Serial.print(gga.latitude);
    Serial.print(" ° | Num Sat. ");
    Serial.print(gga.num_satellites);
    Serial.print(" | HDOP =  ");
    Serial.print(gga.hdop);
    Serial.print(" m | Altitude ");
    Serial.print(gga.altitude);
    Serial.print(" m | Geoidal Separation ");
    Serial.print(gga.geoidal_separation);
    Serial.print(" m");
  }

  Serial.println();
}
