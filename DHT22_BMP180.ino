// Sketch that combines measurements from DHT22 and BMP180 sensors
// and prints them to serial.
// Aris Tzermias (github.com/tzermias)

// Pinout on Arduino Leonardo/Yun.
// Pin 2: SDA (BMP180)
// Pin 3: SCL (BMP180)
// Pin 4: DHT22

#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>
#include <Process.h>

//TFT
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

#define DHTPIN 4     
#define ALTITUDE 115     // My altitude
#define DHTTYPE DHT22    // DHT 22  (AM2302)
#define DELAY 5000       // Delay between measurements in ms

#define RED  0xF800
#define BLUE 0x001F
#define YELLOW 0xFFE0

// TFT Pinout
#define __CS 10
#define __DC 9

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// Initialize BMP180
SFE_BMP180 bmp;

// Initialize TFT
TFT_ILI9163C tft = TFT_ILI9163C(__CS, __DC);

void setup() {
  Serial.begin(9600);
  //Initialize TFT
  tft.begin();
  tft.fillScreen();
  
  tft.println("Screen initialized");
  
 
  // Initialize DHT
  dht.begin();
  tft.println("DHT initialized");
  
  // Initialize BMP180
  if(!bmp.begin()) {
    Serial.print("Error initializing BMP180!");
    while(1);
  }
  tft.println("BMP180 initialized");
  
  //Initialize Bridge
  Bridge.begin();
}

void loop() {
  char status;
  double T,P;
  float h,t,hi;
  Process p;
  
  // Read humidit
  h = dht.readHumidity();
  // Read temperature (Celsius)
  t = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  hi = dht.computeHeatIndex(dht.readTemperature(true), h);
  
  // Temperature from BMP180
  status = bmp.startTemperature();
  if (status == 0) {
    Serial.println("Failed to measure temperature from BMP180");
    return;
  }
  delay(status);
  status = bmp.getTemperature(T);
  
  // Pressure
  status = bmp.startPressure(3);
  if (status == 0) {
    Serial.println("Failed to measure pressure from BMP180");
    return;
  }
  delay(status);
  status=bmp.getPressure(P,T);
  
  // Print Values
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" °C ");
  Serial.print("Heat index: ");
  Serial.print(hi-32);
  Serial.print(" °C ");
  Serial.print("Barometric: ");
  Serial.print(P,2);
  Serial.print(" mb ");
  Serial.print("Temp2: ");
  Serial.print(T);
  Serial.println(" °C");
  
  
  tft.fillScreen();
  tft.setCursor(0,0);
  tft.setTextColor(RED);
  tft.setTextSize(1);
  tft.println("Temperature");
  tft.print(t);
  tft.print(" oC (");
  tft.print(T);
  tft.println(" oC)\n");
  tft.setTextColor(BLUE);
  tft.setTextSize(1.3);
  tft.println("Humidity");
  tft.print(h);
  tft.println(" %\n");
  tft.setTextColor(YELLOW);
  tft.setTextSize(1);
  tft.println("Bar. Pressure");
  tft.print(P, 2);
  tft.println(" mb\n");
  
  //Check connection to the internet
  tft.setCursor(0,120);
  p.runShellCommand("/usr/bin/pretty-wifi-info.lua | grep IP | cut -d: -f2 | tr -d ' '");
  while(p.available()>0){
    char c = p.read();
    tft.print(c);
    Serial.print(c); 
  }
  
  // Add delay between measurements
  delay(DELAY);
}
