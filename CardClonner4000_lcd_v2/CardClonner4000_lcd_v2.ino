/*
 * 
 * All the resources for this project: https://www.hackster.io/Aritro
 * Modified by Aritro Mukherjee
 * 
 * 
 */
 
#include <SPI.h>
#include <MFRC522.h>
#include <MFRC522Hack.h>
#include <Wire.h> // библиотека для управления устройствами по I2C 
#include <LiquidCrystal_I2C.h> // подключаем библиотеку для LCD 1602

LiquidCrystal_I2C lcd(0x27,20,2); // присваиваем имя lcd для дисплея 20х2
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522Hack mfrc522Hack(&mfrc522);  // Create MFRC522Hack instance.
int buttonReadPin = 2;
int buttonWritePin = 3;
//Variables
int buttonStateRead = 0;
int buttonStateWrite = 0;
int flag=0;

//const char* UIDs[]={"E3 DE 72 89"};

 
void setup() 
{
  lcd.begin(); // инициализация LCD дисплея
  lcd.noBacklight();//выключить подсветку
  //lcd.backlight(); // включение подсветки дисплея
  //lcd.setCursor(0,0); // ставим курсор на 1 символ первой строки
  //lcd.print("I LOVE"); // печатаем сообщение на первой строке
  //lcd.setCursor(0,1);  // ставим курсор на 1 символ второй строки
  //lcd.print("ARDUINO"); // печатаем сообщение на второй строке
  
  pinMode(buttonReadPin, INPUT_PULLUP);  
  pinMode(buttonWritePin, INPUT_PULLUP); 
  //digitalWrite(Relay, HIGH);  // реле выключено
  
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();

}
void loop() 
{
  //Read button state (pressed or not pressed?)
  buttonStateRead = digitalRead(buttonReadPin);
  buttonStateWrite = digitalRead(buttonWritePin);
  byte buffer_uid[4];
  


  //If button pressed...
  if (buttonStateRead == LOW) { 
       Serial.println("read metka!");
       //read_metka();
       read_metka(buffer_uid);//Read(10, buffer);
    }    

if (buttonStateWrite == LOW) { 
       Serial.println("write metka!");
       write_metka(buffer_uid);
    }    

  delay(200); //Small delay
  //write

} 



byte  *read_metka(byte *metka_tag)
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  //byte newUid[4]= {};
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
     metka_tag[i]=mfrc522.uid.uidByte[i];
  }
  //
  Serial.println();
  Serial.print("newuid: ");
  for (byte i = 0; i <4; i++) 
   {
     Serial.print(metka_tag[i] < 0x10 ? " 0" : " ");
     Serial.print(metka_tag[i], HEX);
   }
  //
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  Serial.println(content);
  lcd.display();//включение дисплея
  lcd.backlight(); // включение подсветки дисплея
  lcd.setCursor(0,0);//курсор в положение 0 0
  lcd.print("UID tag is");// печатаем авторизован на экране
  lcd.setCursor(0,1);//курсор на 2 строку дисплея
  lcd.print(content);
  delay(5000);
  lcd.noBacklight();//выключить подсветку
  lcd.clear();//очистка дисплея
  return metka_tag;
       
}

void write_metka(byte rfid_uid[4])
{
  //byte newUid[4] = mfrc522.uid;
  if ( mfrc522Hack.MIFARE_SetUid(rfid_uid, (byte)4, true) ) {
    Serial.println(F("Wrote new UID to card."));
  }
  
  // Halt PICC and re-select it so DumpToSerial doesn't get confused
  mfrc522.PICC_HaltA();
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    return;
  }
       
}


