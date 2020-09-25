#include <DHT.h>
#include <SD.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

#define DHTPIN 3     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

#define BLACK 0x0000
#define TEAL   0x575B
#define ORANGE  0xfC00
#define RED     0xf800
#define BLUE    0x001f
#define GREEN   0x07e0
#define YELLOW  0xffe0
#define PURPLE  0xF81F
#define PINK    0xF810
#define WHITE   0xffff
#define CORAL   0xFB48

DHT dht(DHTPIN, DHTTYPE);

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

int Record = 0;
int buttonCount = 0;   // counter for the number of button presses
bool buttonState = 0;         // current state of the button
bool lastButtonState = 0;

const int chipSelect = 4;
const int  buttonPin = 2;

unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
long interval = 5000; //5 second interval

float TempFahrenheit = 0.0;
float Humidity = 0.0;
float HeatIndex = 0.0;

bool firstTemp = true;

String logText = "5 sec";

void setup() {

  pinMode(buttonPin, INPUT);

  // Serial.begin(9600);
  // Serial.println("Working...");

  dht.begin();
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(ST7735_BLACK);

  //   Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  while (!SD.begin(chipSelect)) (1);
}
//  Serial.println("Card Initialized.");

void loop() {

  tft.invertDisplay(false); //why is this here? can I move this to setup?? look into this

  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    Serial.println(buttonState);
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      //clear log text from prior case
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.setCursor(85, 150);
      tft.print(logText);
      switch (buttonCount)
      {
        case 0:
          interval = 30000;
          logText = "30 sec";
          break;

        case 1:
          interval = 60000;
          logText = "1 min";
          break;

        case 2:
          interval = 300000;
          logText = "5 min";
          break;

        case 3:
          interval = 1800000;
          logText = "30 min";
          break;

        case 4:
          interval = 3600000;
          logText = "1 hr";
          break;

        default:
          interval = 5000;
          logText = "5 sec";
          break;
      }
      tft.setTextColor(GREEN);
      tft.setTextSize(1);
      tft.setCursor(85, 150);
      tft.print(logText);
      Serial.println(logText);
      buttonCount = (++buttonCount) % 6;
      delay(250);
    }
    lastButtonState = buttonState;
  }

  currentMillis = millis();

  if (firstTemp == true || currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;
    // clear the last temp
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 25);
    tft.println(TempFahrenheit);
    // clear the last humidity
    tft.setCursor(10, 60);
    tft.println(Humidity);
    // clear the last heat index
    tft.setCursor(10, 95);
    tft.println(HeatIndex);
    // clear the "record"
    tft.setTextColor(BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 130);
    tft.println(Record);

    TempFahrenheit = dht.readTemperature(true);
    Humidity = dht.readHumidity(true);
    HeatIndex = dht.computeHeatIndex(TempFahrenheit, Humidity);
    Record++;

    //write temp reading to to SD card
    File sdcard_file = SD.open("data.txt", FILE_WRITE);
    // if the file is available, write to it:
    if (sdcard_file) {
      if (firstTemp) {
        sdcard_file.println("Record, Log Interval, Millis, Temp (F), Humidity (%), Heat Index (F)");
      }

      sdcard_file.print(Record);
      sdcard_file.print(",");
      sdcard_file.print(logText);
      sdcard_file.print(",");
      sdcard_file.print(currentMillis);
      sdcard_file.print(",");
      sdcard_file.print(TempFahrenheit);
      sdcard_file.print(",");
      sdcard_file.print(Humidity);
      sdcard_file.print(",");
      sdcard_file.println(HeatIndex);
      sdcard_file.close();
    }

    if (firstTemp)
    {
      //label log interval
      tft.setTextColor(GREEN);
      tft.setTextSize(1);
      tft.setCursor(5, 150);
      tft.print("Log Interval:");

      tft.setCursor(85, 150);
      tft.print(logText);

      // Label Temperature
      tft.setTextColor(ORANGE);

      tft.setCursor(10, 10);
      tft.println("Temperature:");

      // Label Humidity
      tft.setTextColor(BLUE);

      tft.setCursor(10, 45);
      tft.println("Humidity:");

      // Label Heat Index
      tft.setTextColor(WHITE);

      tft.setCursor(10, 80);
      tft.println("Heat Index:");

      // Label "Record
      tft.setTextColor(CORAL);

      tft.setCursor(10, 115);
      tft.println("Record #");
    }
    // print the current temp
    tft.setTextColor(PURPLE);
    tft.setTextSize(2);
    tft.setCursor(10, 25);
    tft.println(TempFahrenheit);

    // print the current humidity
    tft.setTextColor(GREEN);
    tft.setCursor(10, 60);
    tft.println(Humidity);

    // print the current heat index
    tft.setTextColor(YELLOW);
    tft.setCursor(10, 95);
    tft.println(HeatIndex);

    // print the current "record"
    tft.setTextColor(PINK);
    tft.setCursor(10, 130);
    tft.println(Record);

    firstTemp = false;
  }
}
