/*
 * Arduino Thermometer using DS18B20 sensor and SSD1306 OLED display.
 * This is a free software with NO WARRANTY.
 * https://simple-circuit.com/
 */
 
#include <Wire.h>              // include Arduino wire library (required for I2C devices)
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_SSD1306.h>  // include Adafruit SSD1306 OLED display driver
 
#define OLED_RESET  4    // define display reset pin
Adafruit_SSD1306 display(OLED_RESET);
 
// define DS18B20 data pin connection
#define DS18B20_PIN   A0
 
void setup(void)
{
  delay(1000);  // wait a second
 
  // initialize the SSD1306 OLED display with I2C address = 0x3D
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
 
  // clear the display buffer.
  display.clearDisplay();
 
  display.setTextSize(1);   // text size = 1
  display.setTextColor(WHITE, BLACK);  // set text color to white and black background
  display.setCursor(13, 0);            // move cursor to position (13, 0) pixel
  display.print("ARDUINO + DS18B20");
  display.setCursor(28, 14);
  display.print("SSD1306 OLED");
  display.setCursor(29, 33);
  display.print("TEMPERATURE:");
  display.display();        // update the display
  display.setTextSize(2);   // text size = 2
}
 
unsigned int ds18b20_temp;
char _buffer[11];
 
void loop()
{    
  display.setCursor(1, 50);
  
  if(ds18b20_read(&ds18b20_temp))
  {
    if (ds18b20_temp & 0x8000)          // if temperature < 0
    {
      ds18b20_temp = ~ds18b20_temp + 1;  // change temperature value to positive form
      sprintf(_buffer, "-%02u.%04u C", (ds18b20_temp/16) % 100, (ds18b20_temp & 0x0F) * 625);
    }
    
    else
    {      // otherwise (temperature >= 0)
      if (ds18b20_temp/16 > 100)          // if temperature >= 100 °C
        sprintf(_buffer, "%03u.%04u C", ds18b20_temp/16, (ds18b20_temp & 0x0F) * 625);
      else      // otherwise ( 0 <= temperature < 100)
        sprintf(_buffer, " %02u.%04u C", ds18b20_temp/16, (ds18b20_temp & 0x0F) * 625);
    }
 
    display.print(_buffer);
    display.drawCircle(103, 52, 2, WHITE);     // put degree symbol ( ° )
  
  }
 
  else
    display.print("   ERROR  ");
 
  display.display();
 
  delay(1000);  // wait a second
 
}
 
bool ds18b20_start()
{
  bool ret = 0;
  digitalWrite(DS18B20_PIN, LOW);  // send reset pulse to the DS18B20 sensor
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(500);          // wait 500 us
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(100);          // wait to read the DS18B20 sensor response
  if (!digitalRead(DS18B20_PIN))
  {
    ret = 1;                  // DS18B20 sensor is present
    delayMicroseconds(400);   // wait 400 us
  }
  return(ret);
}
 
void ds18b20_write_bit(bool value)
{
  digitalWrite(DS18B20_PIN, LOW);
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(2);
  digitalWrite(DS18B20_PIN, value);
  delayMicroseconds(80);
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(2);
}
 
void ds18b20_write_byte(byte value)
{
  byte i;
  for(i = 0; i < 8; i++)
    ds18b20_write_bit(bitRead(value, i));
}
 
bool ds18b20_read_bit(void)
{
  bool value;
  digitalWrite(DS18B20_PIN, LOW);
  pinMode(DS18B20_PIN, OUTPUT);
  delayMicroseconds(2);
  pinMode(DS18B20_PIN, INPUT);
  delayMicroseconds(5);
  value = digitalRead(DS18B20_PIN);
  delayMicroseconds(100);
  return value;
}
 
byte ds18b20_read_byte(void)
{
  byte i, value;
  for(i = 0; i < 8; i++)
    bitWrite(value, i, ds18b20_read_bit());
  return value;
}
 
bool ds18b20_read(int *raw_temp_value)
{
  if (!ds18b20_start())  // send start pulse
    return(0);
  ds18b20_write_byte(0xCC);   // send skip ROM command
  ds18b20_write_byte(0x44);   // send start conversion command
  while(ds18b20_read_byte() == 0);  // wait for conversion complete
  if (!ds18b20_start())             // send start pulse
    return(0);                      // return 0 if error
  ds18b20_write_byte(0xCC);         // send skip ROM command
  ds18b20_write_byte(0xBE);         // send read command
  
  *raw_temp_value = ds18b20_read_byte();   // read temperature LSB byte and store it on raw_temp_value LSB byte
  *raw_temp_value |= (unsigned int)(ds18b20_read_byte() << 8);   // read temperature MSB byte and store it on raw_temp_value MSB byte
  
  return(1);  // OK --> return 1
}
 
// end of code.
