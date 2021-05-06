unsigned long cur_time, old_time;
#include <SPI.h>              // include libraries
#include <LoRa.h>

const long frequency = 923E6;  // LoRa Frequency

const int csPin = 5;          // LoRa radio chip select
const int resetPin = 4;        // LoRa radio reset
const int irqPin = 13;
          
#include <ArduinoJson.h>
String buf_message;
String message;
StaticJsonDocument<200> doc_send;
String json_lora;
const int id[] = {1,2,3,4,5,6,
                  7,8,9,10};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  LoRa.setPins(csPin, resetPin, irqPin);
  while (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    delay(1000);                       // if failed, do nothing
  }
  delay(1000);
  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Gateway");
  Serial.println("Only receive messages from nodes");
  Serial.println("Tx: invertIQ enable");
  Serial.println("Rx: invertIQ disable");
  Serial.println();
  
  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0 ; i < 3; i++){
      doc_send["req"] = id[i];
      serializeJson(doc_send, json_lora); 
      Serial.print("kirim: ");
      Serial.println(json_lora);
      delay(50);
      LoRa_sendMessage(json_lora);
      json_lora = "";      
      tunggu_node(1500);
    }
}

void LoRa_rxMode(){
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {
  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
  buf_message = message;
  message = "";
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}

void tunggu_node(unsigned long _waktu){
  Serial.println("tunggu...");
  cur_time = millis();
  old_time = cur_time;
  while(cur_time - old_time < _waktu ){
    cur_time = millis();
    if(buf_message != message){
      Serial.println(buf_message);
      DynamicJsonDocument doc_get(200);
      deserializeJson(doc_get, buf_message);
      int _id = doc_get["req"];
      float _x;float _y;float _z;
      
      _x = doc_get["x"];
      _y = doc_get["y"];
      _z = doc_get["z"];
      Serial.print(_id); Serial.print(", "); Serial.print(_x);
      Serial.print(", "); Serial.print(_y);
      Serial.print(", "); Serial.println(_z);
      buf_message = message;
      //delay(500);
      break;
    }
  }
}
