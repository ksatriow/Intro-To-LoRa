2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
58
59
60
61
62
63
64
65
66
67
68
69
70
71
72
73
74
75
76
77
78
79
80
81
82
83
84
85
86
87
88
89
90
91
92
93
94
95
96
97
98
99
100
101
102
103
104
105
106
107
108
109
110
111
112
113
114
115
116
117
118
119
120
121
122
123
124
125
126
127
128
129
130
131
132
133
134
135
136
137
  
// Import Wi-Fi library
#include <WiFi.h>
 
//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>
 
//define the pins used by the LoRa transceiver module
#define ss 5
#define rst 14
#define dio0 2
 
#define BAND 433E6    //433E6 for Asia, 866E6 for Europe, 915E6 for North America
 
 
// Replace with your network credentials
String apiKey = "14K8UL2QEK8BTHN6"; // Enter your Write API key from ThingSpeak
const char *ssid = "Alexahome"; // replace with your wifi ssid and wpa2 key
const char *password = "12345678";
const char* server = "api.thingspeak.com";
 
WiFiClient client;
 
 
// Initialize variables to get and save LoRa data
int rssi;
String loRaMessage;
String temperature;
String humidity;
String readingID;
 
 
// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE")
  {
    return temperature;
  }
  else if(var == "HUMIDITY")
  {
    return humidity;
  }
  else if (var == "RRSI")
  {
    return String(rssi);
  }
  return String();
}
 
void setup() {
  Serial.begin(115200);
  int counter;
 
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0); //setup LoRa transceiver module
 
  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(2000);
  }
  if (counter == 10) {
    // Increment readingID on every new reading
    Serial.println("Starting LoRa failed!"); 
  }
  Serial.println("LoRa Initialization OK!");
  delay(2000);
 
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
 
// Read LoRa packet and get the sensor readings
void loop() 
{
  int packetSize = LoRa.parsePacket();
  if (packetSize)
  {
    Serial.print("Lora packet received: ");
    while (LoRa.available())    // Read packet
  {
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData); 
    
    
    int pos1 = LoRaData.indexOf('/');   
    int pos2 = LoRaData.indexOf('&');   
    readingID = LoRaData.substring(0, pos1);                   // Get readingID
    temperature = LoRaData.substring(pos1 +1, pos2);           // Get temperature
    humidity = LoRaData.substring(pos2+1, LoRaData.length());  // Get humidity
  }
  
  rssi = LoRa.packetRssi();       // Get RSSI
  Serial.print(" with RSSI ");    
  Serial.println(rssi);
}
 
  
  if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
   {
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(readingID);
      postStr += "&field2=";
      postStr += String(temperature);
      postStr += "&field3=";
      postStr += String(humidity);
      postStr += "&field4=";
      postStr += String(rssi);
      postStr += "\r\n\r\n\r\n\r\n";
    
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr);
 
    }    
    //delay(30000);
}
