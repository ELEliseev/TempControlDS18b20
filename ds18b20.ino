
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_SSD1306.h>  // include Adafruit SSD1306 OLED display driver
#include <OneWire.h>
#include <DallasTemperature.h>
#define OLED_RESET  4    // define display reset pin
Adafruit_SSD1306 display(OLED_RESET);
OneWire oneWire(15);  // порт подключения датчиков (A1)
DallasTemperature ds(&oneWire);
byte num;              // количество подключенных датчиков

void setup(void)
{
  delay(1000);  // wait a second
  //////////////////////ds18b20////////////////////////////////////////
  ds.begin();                 // инициализация датчика ds18b20
  num = ds.getDeviceCount();   // узнаем количество датчиков
  Serial.print("Number: ");           // выводим полученное количество
  Serial.println(num);
  // initialize the SSD1306 OLED display with I2C address = 0x3D
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
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
void loop()
{
  display.setCursor(1, 50);
  for (byte i = 0; i < num; i++) {
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    ds.requestTemperatures();                       // считываем температуру с датчика
    Serial.print(ds.getTempCByIndex(i));
    Serial.println("C");
  }
  display.print(ds.getTempCByIndex(i));
  display.drawCircle(103, 52, 2, WHITE);     // put degree symbol ( ° )
  display.display();
  delay(1000);  // wait a second
}
