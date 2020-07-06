
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SFE_BMP180.h>
#include <Wire.h>

#define DHTPIN A3
#define DHTTYPE    DHT11
SFE_BMP180 pressure;
#define ALTITUDE 6.7
#define GAS    A2
int gasLevel = 0;

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

void setup() {
  Serial.begin(9600);
   pinMode(GAS,INPUT);
  dht.begin();
  Serial.println(F("LoRa SensNet"));
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  delayMS = sensor.min_delay / 1000;
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {    Serial.println("BMP180 init fail\n\n");
    while(1);
  }
  Serial.println(F("------------------------------------"));
  Serial.println(F("Starting to LoRa SensNet"));
  Serial.println(F("------------------------------------"));
  Serial.println();
}

void loop() {
  char status;
  double T,P,p0,a;
  delay(delayMS);
  sensors_event_t event;
  Serial.println(F("------------------------------------"));
  Serial.println(F("Reading Values"));
  Serial.println();
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Actual Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
  Serial.print("Def. altitude: ");
  Serial.print(ALTITUDE,0);
  Serial.println(" meters");
  //Serial.print(ALTITUDE*3.28084,0);
  //Serial.println(" feet");
  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      Serial.print("Feels like: ");
      Serial.print(T,2);
      Serial.println(F("°C"));
      //Serial.print((9.0/5.0)*T+32.0,2);
      //Serial.println(" deg F");
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          Serial.print("Absolute pressure: ");
          //Serial.print(P,2);
          //Serial.print(" mb, ");
          // Serial.print(P*0.0295333727,2);
          // Serial.println(" inHg");
          Serial.print(P/1013,2);
          Serial.println(" ATM");

          p0 = pressure.sealevel(P,ALTITUDE);
          Serial.print("Relative (sea-level) pressure: ");
          // Serial.print(p0,2);
          // Serial.print(" mb, ");
          // Serial.print(p0*0.0295333727,2);
          // Serial.println(" inHg");
          Serial.print(p0/1013,2);
          Serial.println(" ATM");
          a = pressure.altitude(P,p0);
          Serial.print("Computed altitude: ");
          Serial.print(a,0);
          Serial.println(" meters");
          // Serial.print(a*3.28084,0);
          // Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
  gasLevel = analogRead(GAS);
  Serial.print("Pollution level: ");
  Serial.print(gasLevel);
  Serial.println(" PM2.5");
  Serial.println(F("------------------------------------"));

  delay(5000);
}
