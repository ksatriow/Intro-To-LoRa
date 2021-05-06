// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


#include <SPI.h>
#include <LoRa.h>

const long frequency = 923E6;  // LoRa Frequency

const int resetPin = 5;        // LoRa radio reset
const int csPin = 6;          // LoRa radio chip select
const int irqPin = 3;          // interrupt pin

#include <ArduinoJson.h>
StaticJsonDocument<200> doc;
String json_lora;
String buf_message;
String message;

const int id = 1;

void setup(void)
{
  // start serial port
  Serial.begin(115200);
  sensors.begin();

  LoRa.setPins(csPin, resetPin, irqPin);
  while (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed.");
    delay(1000);// if failed, do nothing
  }
  Serial.println("LoRa sukses");

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();

  /*while(1){
    Serial.println("kirim");
    LoRa_sendMessage("bismillah");
    delay(500);
  }
  */
}

void loop(void)
{
  if (buf_message != message) {
    Serial.print("JSON: ");
    Serial.println(buf_message);
    DynamicJsonDocument doc(200);
    deserializeJson(doc, buf_message);
    int req = doc["req"];
    if (req == id) {
        doc["suhu"] = 32.6;
        serializeJson(doc, json_lora);
        Serial.println(json_lora);
        LoRa_sendMessage(json_lora);
        json_lora = "";
        
      /*sensors.requestTemperatures(); // Send the command to get temperatures
      float tempC = sensors.getTempCByIndex(0);
      if (tempC != DEVICE_DISCONNECTED_C)
      {
        doc["suhu"] = tempC;
        serializeJson(doc, json_lora);
        Serial.println(json_lora);
        LoRa_sendMessage(json_lora);
        json_lora = "";
      }
      */
    }
    buf_message = message;
  }


}

void LoRa_rxMode() {
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode() {
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}

void onReceive(int packetSize) {

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
  buf_message = message;
  message = "";

}
