#include "SPI.h"
#include "MFRC522.h"

//Starting from right to left
//1 - 5v
//2 - 9
//3 - ground

//5 - 12
//6 - 11
//7 - 13
//8 - 10
//LED - pin 6
//LED2 - pin 5

#define SS_PIN 10
#define RST_PIN 9
#define LED_PIN 8
#define LED_PIN2 7

MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    rfid.PCD_Init();
    pinMode(LED_PIN, OUTPUT);
    pinMode(LED_PIN2, OUTPUT);
    
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    return;

  // Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  String strID = "";
  for (byte i = 0; i < 4; i++) {
    strID +=
    (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
    String(rfid.uid.uidByte[i], HEX) +
    (i!=3 ? ":" : "");
  }
  strID.toUpperCase();

  Serial.print("Tap card key: ");
  Serial.println(strID);

  if (strID.indexOf("C5:A2:AE:1C") >= 0) {
    digitalWrite(LED_PIN, HIGH);
  } 
  if (strID.indexOf("05:F9:F8:C2") >= 0) {
    digitalWrite(LED_PIN, LOW);
  }
if (strID.indexOf("F3:A6:B2:89") >= 0){digitalWrite(LED_PIN2, HIGH);}
if (strID.indexOf("7E:4E:A1:59") >= 0){digitalWrite(LED_PIN2, LOW);}
// 
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
