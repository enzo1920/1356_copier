#include <OneWire.h>
 
/* 
 * test 1-Wire whith DS1990A 
 */
 
OneWire  ds(10);  // на  digital pin 10
byte addr[8];

void setup(void) {
  Serial.begin(9600);
}
 
void loop(void) {

if(ds.search(addr)) {// Если устройство подключено - считываем
  for(int i=7; i>-1; i--){// Запускаем цикл печати данных из массива
      Serial.print(addr[i], HEX);// Печатаем нужный байт в шестнадцатиричном виде
      Serial.print(" ");}// Печатаем пробел
      Serial.println();// В конце цикла переводим строку
  ds.reset_search();
  }
}
