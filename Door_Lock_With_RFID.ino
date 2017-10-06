/*
 
*/

//LCD config
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Keypad.h>
#define RST_PIN   9
#define SS_PIN    10

LiquidCrystal_I2C lcd(0x27,20,4);
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myservo;

//Variables
int mot_min = 1;   //min servo angle  (set yours)
int mot_max = 90; //Max servo angle   (set yours)
int character = 0;
int activated =0;
char Str[16] = {' ', ' ', ' ', ' ', ' ', ' ', '-', '*', '*', '*', ' ', ' ', ' ', ' ', ' ', ' '};  


byte readCard[4];
char* myTags[100] = {};
int tagsCount = 0;
String tagID = "";
boolean successRead = false;
boolean correctTag = false;
boolean doorOpened = false;

//Keypad config
const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {8,7,6,5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {4,3,2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

  void setup(){
    Serial.begin(9600);
  SPI.begin();       
  mfrc522.PCD_Init();
  myservo.attach(10); //attach the servo to pin D10
  //Init the screen and print the first text
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Waiting For");
  lcd.setCursor(3, 1);
  lcd.print("Master Tag!");
  //put the servo in the close position first
  myservo.write(mot_min);  
  
  while (!successRead) {
    successRead = getID();
    if ( successRead == true) {
      myTags[tagsCount] = strdup(tagID.c_str()); // Sets the master tag into position 0 in the array
      lcd.clear();
      lcd.setCursor(0, 0);
      tagsCount++;
      lcd.print("Master Tag Set!");
      //Serial.print("Master Tag Set!");  
      delay(1000);
    }
  }
  successRead = false;
  defaultText;
}

uint8_t getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
    return 0;
  }
  tagID = "";
  for ( uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}
void defaultText()
{
  lcd.print("    PASSWORD");
  //Serial.print("Password");  
  lcd.setCursor(0,1);
  lcd.print("      -***     ");
  //Serial.print("-***");  
}
void correct(){
  doorOpened = true;
      myservo.write(mot_max);
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("ACCEPTED");
      //Serial.print("ACCEPTED");
      activated = 2;
      
      lcd.clear();    
      lcd.setCursor(4,0);
      lcd.print("WELLCOME");
      //Serial.print("WELLCOME");
      delay(500);
      lcd.setCursor(2,1);
      lcd.print("ARSHIA'S ROOM");
      //Serial.print("ARSHIA'S ROOM");
      delay(1000);

      lcd.clear();    
      lcd.setCursor(3,0);
      lcd.print("DOOR  OPEN");
      //Serial.print("DOOR  OPEN");
      lcd.setCursor(2,1);
      lcd.print("ARSHIA'S ROOM");
      //Serial.print("ARSHIA'S ROOM");
      }
void denied(){
  doorOpened = false;
  lcd.clear();    
      lcd.setCursor(1,0);
      lcd.print("PASSCODE ERROR");
      //Serial.print("PASSCODE ERROR");
      lcd.setCursor(3,1);
      lcd.print("TRY  AGAIN");
      //Serial.print("TRY  AGAIN");
      character=0;
      Str[6]= '-';
      Str[7]= '*'; 
      Str[8]= '*'; 
      Str[9]= '*';
      Str[10]= ' ';
      activated = 0;
      lcd.clear();    
      lcd.setCursor(4,0);
      lcd.print("PASSWORD");
      //Serial.print("PASSWORD");
      lcd.setCursor(0,1);
      lcd.print(Str);   
      }
void loop(){
  
  if(!doorOpened)
  {if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
      return;
    }
    if ( ! mfrc522.PICC_ReadCardSerial()) {   //Since a PICC placed get Serial and continue
      return;
    }
    tagID = "";
    // The MIFARE PICCs that we use have 4 byte UID
    for ( uint8_t i = 0; i < 4; i++) {  //
      readCard[i] = mfrc522.uid.uidByte[i];
      tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable
    }
    tagID.toUpperCase();
    mfrc522.PICC_HaltA(); // Stop reading
    correctTag = false;
    // Checks whether the scanned tag is the master tag
    if (tagID == myTags[0]) {
      lcd.clear();
      lcd.print("Program mode:");
      //Serial.print("Program mode");
      lcd.setCursor(0, 1);
      lcd.print("Add/Remove Tag");
      //Serial.print("Add/Remove Tag");
      while (!successRead) {
        successRead = getID();
        if ( successRead == true) {
          for (int i = 0; i < 100; i++) {
            if (tagID == myTags[i]) {
              myTags[i] = "";
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("  Tag Removed!");
              //Serial.print("Tag Removed!");
              defaultText();
              return;
            }
          }
          myTags[tagsCount] = strdup(tagID.c_str());
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("   Tag Added!");
          //Serial.print("   Tag Added!");
          defaultText();
          tagsCount++;
          return;
        }
      }
    }
    successRead = false;
    // Checks whether the scanned tag is authorized
    for (int i = 0; i < 100; i++) {
      if (tagID == myTags[i]) {
        correct();
        defaultText();
        correctTag = true;
      }
    }
    if (correctTag == false) {
      denied();
      defaultText();
    }
  }
///////////////KEYPAD OPEN/CLOSE////////////  
  char customKey = customKeypad.getKey(); //this function reads the presed key
  Serial.println(customKey);
  if (customKey){

    if (character ==0)
    {  
    Serial.print(customKey);
    Str[6]= customKey;   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    PASSWORD");
   // Serial.print("Password");    
    lcd.setCursor(0,1);
    lcd.print(Str);
  //  Serial.print(Str);
   
    }

    if (character ==1)
    {  
    Serial.print(customKey);
    Str[7]= customKey;   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    PASSWORD");
    //Serial.print("Password");    
    lcd.setCursor(0,1);
    lcd.print(Str);
   
    }

    if (character ==2)
    {  
    Serial.print(customKey);
    Str[8]= customKey;   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    PASSWORD");
    //Serial.print("Password");      
    lcd.setCursor(0,1);
    lcd.print(Str);
   
    }

    if (character ==3)
    {  
    Serial.print(customKey);
    Str[9]= customKey;   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    PASSWORD");
    //Serial.print("Password");      
    lcd.setCursor(0,1);
    lcd.print(Str);
   
    }

    if (character ==4)
    {  
    Serial.print(customKey);
    Str[10]= customKey;
    activated=1;
   
    }
    character=character+1;
  }

  if (activated == 1)
    {
/*Change your password below!!! 
Change each of Str[6], Str[7], Str[8], Str[9]*/

    if(Str[10]='A' && character==5 && Str[6]=='2' && Str[7]=='5' && Str[8]=='8' && Str[9]=='0' )
    {
     correct(); 
    }
    else
    {
     denied();
    }
  }
  if (activated == 2)
    {
    if(customKey == '*' )
    {
      myservo.write(mot_min);
      activated = 0;
      character=0;
      Str[6]= '-';
      Str[7]= '*'; 
      Str[8]= '*'; 
      Str[9]= '*';
      Str[10]= ' ';   
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("    PASSWORD");
      //Serial.print("Password");      
      lcd.setCursor(0,1);
      lcd.print(Str);
     doorOpened = false;
    }
  }  
 if (customKey){
    Serial.println(customKey);
  }
lcd.clear();
}




