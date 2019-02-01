#include <OneWire.h>  // подключаем библиотеку
const int pin = 10;   // объявляем номер пина
OneWire iButton(pin); // объявляем объект OneWire на 10-ом пине

// номер ключа, который мы хотим записать в iButton:
//A 0 0 12 F1 4D 3F 1 
byte key_to_write[] = { 0x01, 0x3F, 0x4D, 0xF1, 0x12, 0x00, 0x00, 0x0A };

void setup(void) { 
  Serial.begin(9600); 
  pinMode(pin, OUTPUT);  
}

void loop(void) {
  delay(1000); // задержка на 1 сек  
  iButton.reset(); // сброс устройства 1-wire
  delay(50); 
  iButton.write(0x33); // отправляем команду "чтение"

  byte data[8]; // массив для хранения данных ключа
  iButton.read_bytes(data, 8); // считываем данные приложенного ключа, 8х8=64 бита
  for(int i=7; i>-1; i--){// Запускаем цикл печати данных из массива.
    Serial.print(data[i], HEX);// Печатаем нужный байт в шестнадцатеричном виде
    Serial.print(" ");}// Печатаем пробел
  
  if ( OneWire::crc8(data, 7) != data[7] ) { // проверяем контрольную сумму приложенного ключа
      Serial.println("CRC error!"); // если CRC не верна, сообщаем об этом
      return; // и прерываем программу
  }

  if (data[0] & data[1] & data[2] & data[3] & data[4] & data[5] & data[6] & data[7] == 0xFF) {
    return; // если ключ не приложен к считывателю, прерываем программу и ждём, пока будет приложен
  }

  Serial.print("Start programming..."); // начало процесса записи данных в ключ
  
  for (int i = 0; i < 8; i++) {
    iButton.reset(); // сброс ключа и формирование 4-х байт для записи в ключ
    data[0] = 0x3C; // отправляем команду "запись"
    data[1] = i; // указываем байт для записи
    data[2] = 0;
    data[3] = key_to_write[i]; 
    iButton.write_bytes(data, 4); // записываем i-ый байт в ключ
  
    uint8_t b = iButton.read(); // считываем байт из ключа
    
    if (OneWire::crc8(data, 4) != b) { // при ошибке контрольной суммы
        Serial.println("Error while programming!"); // сообщаем об этом
        return; // и отменяем запись ключа
    }
    send_programming_impulse(); // если всё хорошо, посылаем импульс для записи i-го байта в ключ
  }
  Serial.println("Success!"); // сообщение об успешной записи данных в ключ  
}

// Инициализация записи данных в ключ-таблетку iButton:
void send_programming_impulse() { 
  digitalWrite(pin, HIGH); 
  delay(60);
  digitalWrite(pin, LOW); 
  delay(5);
  digitalWrite(pin, HIGH); 
  delay(50); 
}
