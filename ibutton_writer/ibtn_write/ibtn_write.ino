#include <OneWire.h>



#define pin 10
OneWire ibutton (pin); // Пин D10 для подлючения iButton (Data)
byte addr[8];
byte ReadID[8] = { 0x01, 0x3F, 0x4D, 0xF1, 0x12, 0x00, 0x00, 0x0A }; // "пашин" ключ. A 0 0 12 F1 4D 3F 1 

const int buttonPin = 6;
//const int ledPin = 13;
int buttonState = 0;
int writeflag = 0;
int readflag = 0;

void setup() {
  //pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  //Serial.begin(115200);
  Serial.begin(9600);// Запускаем порт
}

void loop() {

  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    readflag = 1;
    writeflag = 1;
    //digitalWrite(ledPin, HIGH);
  }
  if (!ibutton.search (addr)) {
    ibutton.reset_search();
    delay(50);
    return;
  }

  //digitalWrite(ledPin, HIGH);
  delay(50);

  for (byte x = 0; x < 8; x++) {
    Serial.print(addr[x], HEX);
    if (readflag == 0) {
      ReadID[x] = (addr[x]);
    }
    Serial.print(":");
  }

  byte crc; // Проверка контрольной суммы
  crc = ibutton.crc8(addr, 7);
  Serial.print("CRC: ");
  Serial.println(crc, HEX);
  //digitalWrite(ledPin, LOW);

  if ((writeflag == 1) or (Serial.read() == 'w')) {
    ibutton.skip(); 
    ibutton.reset(); 
    ibutton.write(0x33);
    Serial.print("  ID before write:");
    for (byte x = 0; x < 8; x++) {
      Serial.print(' ');
      Serial.print(ibutton.read(), HEX);
    }
    // send reset
    ibutton.skip();
    ibutton.reset();
    // send 0xD1
    ibutton.write(0xD1);
    // send logical 0
    digitalWrite(pin, LOW); pinMode(pin, OUTPUT); delayMicroseconds(60);
    pinMode(pin, INPUT); digitalWrite(pin, HIGH); delay(10);

    Serial.print('\n');
    Serial.print("  Writing iButton ID:\n    ");
    byte newID[8] = { (ReadID[0]), (ReadID[1]), (ReadID[2]), (ReadID[3]), (ReadID[4]), (ReadID[5]), (ReadID[6]), (ReadID[7]) };
    ibutton.skip();
    ibutton.reset();
    ibutton.write(0xD5);
    for (byte x = 0; x < 8; x++) {
      writeByte(newID[x]);
      Serial.print('*');
    }
    Serial.print('\n');
    ibutton.reset();
    // send 0xD1
    ibutton.write(0xD1);
    //send logical 1
    digitalWrite(pin, LOW); 
    pinMode(pin, OUTPUT); 
    delayMicroseconds(10);
    pinMode(pin, INPUT); 
    digitalWrite(pin, HIGH); delay(10);
    writeflag = 0;
    readflag = 0;
    //digitalWrite(ledPin, LOW);
  }
}

int writeByte(byte data) {
  int data_bit;
  for (data_bit = 0; data_bit < 8; data_bit++) {
    if (data & 1) {
      digitalWrite(pin, LOW); pinMode(pin, OUTPUT);
      delayMicroseconds(60);
      pinMode(pin, INPUT); digitalWrite(pin, HIGH);
      delay(10);
    } else {
      digitalWrite(pin, LOW); pinMode(pin, OUTPUT);
      pinMode(pin, INPUT); digitalWrite(pin, HIGH);
      delay(10);
    }
    data = data >> 1;
  }
  return 0;
}