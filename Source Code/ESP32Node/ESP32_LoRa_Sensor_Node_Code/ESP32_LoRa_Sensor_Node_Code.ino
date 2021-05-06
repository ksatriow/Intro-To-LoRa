#include <SPI.h>
#include <LoRa.h>
 
//Libraries for LoRa
#include "DHT.h"
#define DHTPIN 4          //pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);
 
//define the pins used by the LoRa transceiver module
#define ss 5
#define rst 14
#define dio0 2
 
#define BAND 433E6    //433E6 for Asia, 866E6 for Europe, 915E6 for North America
 
//packet counter
int readingID = 0;
 
int counter = 0;
String LoRaMessage = "";
 
float temperature = 0;
float humidity = 0;
 
 
 
//Initialize LoRa module
void startLoRA()
{
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module
 
  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) 
  {
    // Increment readingID on every new reading
    readingID++;
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
}
 
void startDHT()
{
  if (isnan(humidity) || isnan(temperature)) 
  {
  Serial.println("Failed to read from DHT sensor!");
  return;
  }
}
 
void getReadings(){
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.print(F("%  Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C "));
}
 
void sendReadings() {
  LoRaMessage = String(readingID) + "/" + String(temperature) + "&" + String(humidity) ;
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();
  
  Serial.print("Sending packet: ");
  Serial.println(readingID);
  readingID++;
  Serial.println(LoRaMessage);
}
 
void setup() {
  //initialize Serial Monitor
  Serial.begin(115200);
  dht.begin();
  startDHT();
  startLoRA();
}
void loop() {
  getReadings();
  sendReadings();
  delay(500);
}
