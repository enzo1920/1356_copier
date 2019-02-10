// Watch video here: https://www.youtube.com/watch?v=M4mVDnlnzSA

// UIP is a proper library for Arduino Nano Ethernet shield
// NOTE: UIPEthernet library is not needed if you are using Arduino UNO/Duemilanove/Mega/etc.
// UIPEthernet library is used for Arduino Nano Ethernet Shield

#include <UIPEthernet.h> // Used for Ethernet
#include <OneWire.h>


OneWire ds(5); // на пине 10 (нужен резистор 4.7 КОм)

// **** ETHERNET SETTING ****
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x78, 0xEE  };                                       
IPAddress ip(10, 10, 10, 244); 
IPAddress mydns(10,10,10,1);
IPAddress mygw(10,10,10,1);
IPAddress subnet(255,255,255,0);
                      
EthernetServer server(80);

// Relay state and pin
String relay1State = "Off";
const int relay = 7;
String str_temp;

// Client variables 
char linebuf[80];
int charcount=0;

void setup() {

  // Relay module prepared 
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip,mydns,mygw,subnet);
  server.begin();

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());
}



// Display dashboard page with on/off button for relay
// It also print Temperature in C and F
void dashboardPage(EthernetClient &client) {
  str_temp = DS18S20();
  Serial.println(str_temp);
  client.println("<!DOCTYPE HTML><html><head>");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>");                                                             
  client.println("<h3>Arduino Web Server - <a href=\"/\">Refresh</a></h3>");
  client.println("<h4>Temp now is: " + str_temp + "</h4>");
  // Generates buttons to control the relay
  client.println("<h4>Relay 1 - State: " + relay1State + "</h4>");
  // If relay is off, it shows the button to turn the output on          
  if(relay1State == "Off"){
    client.println("<a href=\"/relay1on\"><button>ON</button></a>");
  }
  // If relay is on, it shows the button to turn the output off         
  else if(relay1State == "On"){
    client.println("<a href=\"/relay1off\"><button>OFF</button></a>");                                                                    
  }
  client.println("</body></html>"); 
}

String  DS18S20(){
   byte i;
   byte present = 0;
   byte type_s;
   byte data[12];
   byte addr[8];
   String cels_str;
   float celsius, fahrenheit;
   if (!ds.search(addr)) {
         //Serial.println("No more addresses.");
         //Serial.println();
         ds.reset_search();
         delay(250);
         return;
    }
   Serial.print("ROM =");
   for( i = 0; i < 8; i++) {
        Serial.write(' ');
        Serial.print(addr[i], HEX);
   }
   if (OneWire::crc8(addr, 7) != addr[7]) {
        //Serial.println("CRC is not valid!");
        return;
   }
   Serial.println();
   // первый байт определяет чип
   switch (addr[0]) {
          case 0x10:
                   //Serial.println(" Chip = DS18S20"); // или более старый DS1820
                   type_s = 1;
                   break;
          case 0x28:
                   //Serial.println(" Chip = DS18B20");
                   type_s = 0;
                   break;
          case 0x22:
                   //Serial.println(" Chip = DS1822");
                   type_s = 0;
                   break;
          default:
                   //Serial.println("Device is not a DS18x20 family device.");
                   return;
          }
    ds.reset();
    ds.select(addr);
    ds.write(0x44); // начинаем преобразование, используя ds.write(0x44,1) с "паразитным" питанием
    delay(1000); // 750 может быть достаточно, а может быть и не хватит
    // мы могли бы использовать тут ds.depower(), но reset позаботится об этом
    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE);
    //Serial.print(" Data = ");
    //Serial.print(present, HEX);
    //Serial.print(" ");
    for ( i = 0; i < 9; i++) { // нам необходимо 9 байт
          data[i] = ds.read();
          //Serial.print(data[i], HEX);
          //Serial.print(" ");
        }
    //Serial.print(" CRC=");
    //Serial.print(OneWire::crc8(data, 8), HEX);
    //Serial.println();
    // конвертируем данный в фактическую температуру
    // так как результат является 16 битным целым, его надо хранить в
    // переменной с типом данных "int16_t", которая всегда равна 16 битам,
    // даже если мы проводим компиляцию на 32-х битном процессоре
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
            raw = raw << 3; // разрешение 9 бит по умолчанию
            if (data[7] == 0x10) {
                  raw = (raw & 0xFFF0) + 12 - data[6];
            }
     } 
     else {
            byte cfg = (data[4] & 0x60);
            // при маленьких значениях, малые биты не определены, давайте их обнулим
            if (cfg == 0x00) raw = raw & ~7; // разрешение 9 бит, 93.75 мс
            else if (cfg == 0x20) raw = raw & ~3; // разрешение 10 бит, 187.5 мс
            else if (cfg == 0x40) raw = raw & ~1; // разрешение 11 бит, 375 мс
            //// разрешение по умолчанию равно 12 бит, время преобразования - 750 мс
      }
      celsius = (float)raw / 16.0;
      //fahrenheit = celsius * 1.8 + 32.0;
      //Serial.print(" Temperature = ");
      //Serial.print(celsius);
      //Serial.print(" Celsius, ");
      //Serial.print(fahrenheit);
      //Serial.println(" Fahrenheit");
      cels_str = String(celsius,2);
      
      return cels_str;
}


void loop() {
   // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
       char c = client.read();
       //read char by char HTTP request
        linebuf[charcount]=c;
        if (charcount<sizeof(linebuf)-1) charcount++;
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          dashboardPage(client);
          break;
        }
        if (c == '\n') {
          if (strstr(linebuf,"GET /relay1off") > 0){
            digitalWrite(relay, HIGH);
            relay1State = "Off";
          }
          else if (strstr(linebuf,"GET /relay1on") > 0){
            digitalWrite(relay, LOW);
            relay1State = "On";
          }
          // you're starting a new line
          currentLineIsBlank = true;
          memset(linebuf,0,sizeof(linebuf));
          charcount=0;          
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}
