#include <SoftwareSerial.h>
#include <SPI.h>
#include <LoRa.h>
SoftwareSerial lcd(2, 3);

void setup() {
  Serial.begin(9600);
  lcd.begin(9600);
  while (!Serial);
  if (!LoRa.begin(923E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  clearDisplay();
  setLCDCursorUnder();
}
void loop() {
  setLCDCursor(0);
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      lcd.print((char)LoRa.read());
    }
    lcd.print(" RSSI=");
    lcd.print(LoRa.packetRssi());
    lcd.print(" ");
  }
}
void clearDisplay()
{
  lcd.write(0xFE);
  lcd.write(0x01);
}
void setLCDCursor(byte cursor_position)
{
  lcd.write(0xFE);
  lcd.write(0x80);
  lcd.write(cursor_position);
}
void setLCDCursorUnder()
{
  lcd.write(0xFE);
  lcd.write(0x0C);
}
void setLCDBlink()
{
  lcd.write(0xFE);
  lcd.write(0x0C);
}
