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

#define DHTPIN 4     
#define ALTITUDE 115     // My altitude
#define DHTTYPE DHT22    // DHT 22  (AM2302)
#define DELAY 5000       // Delay between measurements in ms

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// Initialize BMP180
SFE_BMP180 bmp;

void setup() {
  Serial.begin(9600);
 
  // Initialize DHT
  dht.begin();
  
  // Initialize BMP180
  if(!bmp.begin()) {
    Serial.print("Error initializing BMP180!");
    while(1);
  }
}

void loop() {
  char status;
  double T,P;
  float h,t,hi;

  // Add delay between measurements
  delay(DELAY);
  
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
  Serial.print(" *C ");
  Serial.print("Heat index: ");
  Serial.print(hi-32);
  Serial.print(" *C ");
  Serial.print("Humidity: ");
  Serial.print(P,2);
  Serial.print(" mb ");
  Serial.print("Temp2: ");
  Serial.print(T);
  Serial.println(" *C");
}