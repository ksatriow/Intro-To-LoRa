//source : https://diyodemag.com/projects/lora_weather_station


#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"
#define DHTPIN A0
#define DHTTYPE DHT11

const int DRY = 700;
const int WET = 120;
const int DELAY = 173000;//Delay in ms between tx

int count = 0;
int var = 0;
int DHTpin = 4;
int moistPin = 5;
int moist = 0;

DHT dht(DHTPIN, DHTTYPE);
void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(DHTpin, OUTPUT);
  pinMode(moistPin, OUTPUT);
  digitalWrite(DHTpin, LOW);
  digitalWrite(moistPin, LOW);
  if (!LoRa.begin(923E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  dht.begin(); 
}

void loop() {
  // TEMP AND HUMIDITY SENSOR
  digitalWrite(DHTpin, HIGH);
  delay(300);
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();
  digitalWrite(DHTpin, LOW);
  if (isnan(temp)){
    temp = 0;
  }
  if (isnan(humid)){
    humid = 0;
  }
  // MOISTURE SENSOR
  digitalWrite(moistPin, HIGH);
  delay(300);
  moist = analogRead(1);
  digitalWrite(moistPin, LOW);
  moist = map(moist, WET, DRY, 0, 100);
  // COUNTER
  count ++;
  if(count > 999){
    count = 0;
  }
  // LORA PACKET TX
  LoRa.beginPacket();
  LoRa.print("Temprature = " );
  LoRa.print(temp);
  LoRa.print("C");
  LoRa.print("Rel Humidity= ");
  LoRa.print(humid);
  LoRa.print("%");
  LoRa.print("Aprox Moisture= ");
  LoRa.print(moist);
  LoRa.print("%");
  LoRa.print("cnt=");
  LoRa.print(count);
  LoRa.endPacket();
  delay(DELAY);
}
