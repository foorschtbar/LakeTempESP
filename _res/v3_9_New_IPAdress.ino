#include <SPI.h>
#include <LiquidCrystal.h>
#include <string.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <Time.h>
#include <DallasTemperature.h> // http://milesburton.com/index.php/Dallas_Temperature_Control_Library

// Time
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message

// Digital IO port used for one wire interface
int ONE_WIRE_BUS = 8 ;

// Ethernet mac address - this needs to be unique
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// IP-Adress
byte ip[] = { 
  192, 168, 2, 9 };

// IP-Adress
byte router[] = { 
  192, 168, 2, 1 };

// IP-Adress
byte subnet[] = { 
  255, 255, 255, 0 };

// IP address of webserver
//byte server[] = { 85, 13, 147, 172 }; // spaps.de
char server[] = "spaps.de";
//byte server[] = { 85, 13, 131, 162 }; // spaps.de
//byte server[] = { 192, 168, 0, 32}; // local machine

// simple web client to connect
EthernetClient client;

// Version Info
char version[] = "v3.9";

// LCD Setup
LiquidCrystal lcd(6, 7, 2, 3, 4, 5);
//                E, RS, D4, D5, D6, D7

// Buggel-S
byte CharBuggels[8] = {
  B01100,
  B10010,
  B10010,
  B10110,
  B10001,
  B10001,
  B10110,
  B10000,
};

// Grad
byte CharGrad[8] = {
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
  B00000,
};

// Grad
byte CharPackMan[8] = {
  B01110,
  B11011,
  B11110,
  B11100,
  B11110,
  B11111,
  B01110,
  B00000,
};

//highwater
int highwater = 0;

// NW Reset Pin
int DO_RESET_ETH_SHIELD = 9;

// Transmit Failed Counter for Reset
int FAILED_COUNTER = 0;

// LCD Setup
int count_LCD=1;
int timeblinker=0; //Switchs every loop between 0 and 1;

// Millis-Counter for sendData
long sendDataMillis =  -240000; // erster Upload nach 1er Minute (Damit wird currentMillis unten im loop hochgesetzt ('-' + '-' denn gibt plus!)
long sendDataInterval = 300000; // 5 Min (Regelmaesiger Upload)

// Millis-Couter for RequestTemp
long requestTemperaturesMillis = 0; 
long requestTemperaturesInterval = 30000; // 30 sek

// Devices Counter
int numberOfDevices = 0;


// CRLF
#define CRLF "\r\n"

// Setup a oneWire instance to communicate with any OneWire devices (not just MLOCAL >>im/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// Temp Sensor Definition
DeviceAddress OneWire_Sensor1 = {
  0x28, 0x2E, 0x43, 0x6A, 0x02, 0x00, 0x00, 0xD8}
, OneWire_Sensor2 = {
  0x28, 0x4A, 0x82, 0xF9, 0x01, 0x00, 0x00, 0xA5}
, OneWire_Sensor3 = {
  0x28, 0x3A, 0xF5, 0x90, 0x02, 0x00, 0x00, 0xE6}
, OneWire_Sensor4 = {
  0x28, 0xAE, 0x67, 0x6A, 0x02, 0x00, 0x00, 0xD5};

//OneWire_Sensor1 = Wasser {0x28, 0x2E, 0x43, 0x6A, 0x02, 0x00, 0x00, 0xD8}
//OneWire_Sensor2 = Solar {0x28, 0x4A, 0x82, 0xF9, 0x01, 0x00, 0x00, 0xA5}
//OneWire_Sensor3 = SysBoard (283AF590020000E6)
//OneWire_Sensor4 = Luft = {0x28, 0xAE, 0x67, 0x6A, 0x02, 0x00, 0x00, 0xD5}

// 
//DeviceAddress in = {0x28, 0x3A, 0xD6, 0x90, 0x02, 0x00, 0x00, 0x50 }, in2 = {0x28, 0x39, 0xCE, 0x90, 0x02, 0x00, 0x00, 0x4B};
//  DeviceAddress in = {0x28, 0x3A, 0xD6, 0x90, 0x02, 0x00, 0x00, 0x50 }, out = {0x28, 0x91, 0x6B, 0x6A, 0x02, 0x00, 0x00, 0x3D};

void setup()
{
  // Note: Ethernet shield uses digitial IO pins 10,11,12, and 13   
  Serial.begin(9600);
  Serial.print("== LakeTemp ");
  Serial.print(version);
  Serial.println(" ==");

  // LCD begin
  lcd.begin(20, 4);
  lcd.createChar(0, CharBuggels);
  lcd.createChar(1, CharGrad);
  lcd.createChar(2, CharPackMan);

  // LCD Info start
  lcdPrintHeader();
  lcd.setCursor(0,1);
  lcd.print("   Version: ");
  lcd.print(version);

  lcd.setCursor(0,2);
  lcd.print(" Build: 01.01.2017");

  lcd.setCursor(0,3);
  lcd.print("  (c) Fabian Otto  ");
  delay(1500);

  // LCD Info end
  // wait 15 second ethernet shiled
  Serial.println("Warte auf Ethernet Shield... (15s)");
  lcd.clear();
  lcdPrintHeader();
  lcd.setCursor(0,1);
  lcd.print("    Die drahtlose");
  lcd.setCursor(0,2);
  lcd.print(" Netzwerkverbindung");
  lcd.setCursor(0,3);
  lcd.print("wird hergestellt...");
  lcd.blink();
  delay(15000);
  init_ethernet();
  lcd.noBlink();
  // locate devices on the 1W bus
  Serial.println("Suche nach 1W Sensoren...");
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  Serial.print("Es wurden '");
  Serial.print( numberOfDevices );
  Serial.println("' 1W-Sensore gefunden.");

  // select the first sensor   
  for ( int i=0; i<numberOfDevices; i++ )
  {
    DeviceAddress deviceAddress;
    if ( sensors.getAddress(deviceAddress, i) ) 
    {
      Serial.print("Sensor #");
      Serial.print(i);
      Serial.print(" hat Adresse ");
      printAddress(deviceAddress);
      Serial.println();
    }
    else
    {
      Serial.print("Es konnten keine Adressen gefunden werden!"); 
      Serial.println(i);
    }  
  }

  sendData();

}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(dayStr(weekday()));
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(monthShortStr(month()));
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void reset_ethernet() {
  pinMode(DO_RESET_ETH_SHIELD, OUTPUT);      // sets the digital pin as output
  digitalWrite(DO_RESET_ETH_SHIELD, LOW);
  delay(1000);
  digitalWrite(DO_RESET_ETH_SHIELD, HIGH);
  delay(1000);  
  pinMode(DO_RESET_ETH_SHIELD, INPUT);      // sets the digital pin input
  delay(1000);   
}

void init_ethernet() {
  Serial.print("Beginne Ethernet Init. Bitte warten... ");
  Ethernet.begin(mac, ip, router, router, subnet);
  Serial.println("abgeschlossen!");
  /*
 //Serial.println("LOCAL.NW: Ethernet.begin(); #1");
   delay(1000);  
   Ethernet.begin(mac, ip, router, router, subnet);
   //Serial.println("LOCAL.NW: Ethernet.begin(); #2");
   delay(1000);  
   //Serial.println("LOCAL.NW: Starte Webclient auf Port 80");
   //Ethernet client(server, 80);
   //delay(1000);
   */

}

void printIP(Print *stream, byte* ip)
{
  char buf[6];
  stream->print(itoa(ip[0],buf,10));
  stream->print(".");
  stream->print(itoa(ip[1],buf,10));
  stream->print(".");
  stream->print(itoa(ip[2],buf,10));
  stream->print(".");
  stream->print(itoa(ip[3],buf,10));
}

void lcdPrintHeader() {

  lcd.setCursor(0,0);
  lcd.print("LakeTemp ");
  //lcd.write(2);
  lcd.setCursor(11,0);

  if(timeStatus() == timeSet) {
    char buffer[8];
    sprintf(buffer, "%02d:%02d Uhr", hour(), minute());
    lcd.print(buffer);
  } 
  else {
    lcd.print("--:-- Uhr");
  }

  if(timeblinker==0) {
    timeblinker=1;
    lcd.setCursor(13,0);
    lcd.print(":");
  } 
  else {
    timeblinker=0;
    lcd.setCursor(13,0);
    lcd.print(" ");
  }


}

void sendData() {  
  Serial.println("Beginne mit Webserver Upload...");
  // Send the command to get temperatures
  sensors.requestTemperatures(); 

  float Temp_Sensor1 = -128.00;    
  float Temp_Sensor2 = -128.00;
  float Temp_Sensor3 = -128.00;
  float Temp_Sensor4 = -128.00;

  // Sensoren erstmal auf falschen Wert um Fehler durch nicht angeschlossene zu verhindern. Defekte senden eh 127.0
  if(numberOfDevices != 0) {     
    Temp_Sensor1 = sensors.getTempC(OneWire_Sensor1);
    Temp_Sensor2 = sensors.getTempC(OneWire_Sensor2);
    Temp_Sensor3 = sensors.getTempC(OneWire_Sensor3);
    Temp_Sensor4 = sensors.getTempC(OneWire_Sensor4);
  } 
  else  {
    Temp_Sensor1 = -128.00;    
    Temp_Sensor2 = -128.00;
    Temp_Sensor3 = -128.00;
    Temp_Sensor4 = -128.00;
  }

  // Sensor1
  //Serial.print("LOCAL.SENDDATA. Sensor 'Sensor1'");
  //Serial.print(" Temp=");
  //Serial.print(Temp_Sensor1);
  //Serial.print(", Address=");
  //printAddress(OneWire_Sensor1);
  //Serial.println("");

  // Sensor2
  //Serial.print("LOCAL.SENDDATA. Sensor 'Sensor2'");
  //Serial.print(" Temp=");
  //Serial.print(Temp_Sensor2);
  //Serial.print(", Address=");
  //printAddress(OneWire_Sensor2);
  //Serial.println("");

  // Sensor 3
  //Serial.print("LOCAL.SENDDATA. Sensor 'Sensor2'");
  //Serial.print(" Temp=");
  //Serial.print(Temp_Sensor2);
  //Serial.print(", Address=");
  //printAddress(OneWire_Sensor2);
  //Serial.println("");

  lcd.clear();
  lcdPrintHeader();
  lcd.setCursor(0,1);
  lcd.print("      Webupload");
  lcd.setCursor(0,2);
  lcd.print("  to temp.spaps.de");
  //printIP(&lcd, server);
  lcd.setCursor(0,3);
  lcd.print("> connecting...     ");


  Serial.print("Stelle Verbindung zum Webserver her...");

  /** ANFRAGE AN WEBSERVER **/

  if (client.connect(server, 80)) {
    
    Serial.println("hergestellt!");
    lcd.setCursor(0,3);
    lcd.print("                   ");
    lcd.setCursor(0,3);
    lcd.print("> syncing...");
    lcd.blink();

    // Send data to webserver
    // client.print("GET /w006970d/domains/temp.spaps.de/insert.php?value={"); // {
    client.print("GET /insert.php?value={"); // {
    // client.print("GET /root/projekte/laketemp/htdocs/insert.php?value={"); // {
    //{'temps':{'282E436A020000D8':'0.56','284A82F9010000A5':'2.75','283AF590020000E6':'8.56','28AE676A020000D5':'0.88'},'data':{'light':777,'highwater':'0','uptime':'497867552'}} 
    //,"overwater":"yes"}
    client.print("'temps':{'");           // "
    printAddressForWeb(OneWire_Sensor1);       // 0x0x0x0x0x0x
    client.print("':'");
    client.print(Temp_Sensor1);        // 25.1
    client.print("','");
    printAddressForWeb(OneWire_Sensor2);       // 0x0x0x0x0x0x
    client.print("':'");
    client.print(Temp_Sensor2);        // 25.1
    client.print("','");
    printAddressForWeb(OneWire_Sensor3);       // 0x0x0x0x0x0x
    client.print("':'");
    client.print(Temp_Sensor3);        // 25.1
    client.print("','");
    printAddressForWeb(OneWire_Sensor4);       // 0x0x0x0x0x0x
    client.print("':'");
    client.print(Temp_Sensor4);        // 25.1
    client.print("'},'data':{'light':");
    client.print(analogRead(0));
    client.print(",'highwater':'");
    client.print(highwater);
    client.print("','uptime':'");
    client.print(millis());
    client.println("'}} HTTP/1.1"); // }
    client.println("Host: temp.spaps.de");
    client.println("Connection: close");
    client.println();
  } 
  else {
    lcd.setCursor(0,3);
    lcd.print("> no connection!    ");
    Serial.println("keine verbindung!");
    Serial.println("");

    if(FAILED_COUNTER < 1) {
      FAILED_COUNTER++;
      Serial.print("Verbindungsversuche: ");
      Serial.println(FAILED_COUNTER);
      delay(1000);
    } 
    else {
      FAILED_COUNTER=0;
      //Serial.print("LOCAL.SENDDATA: Zu viele Disconnects. Neuinitialisierung starten... ");
      lcd.setCursor(0,2);
      lcd.print("> start auto retry: ");
      lcd.setCursor(0,3);
      lcd.print("> reboot network...");
      lcd.blink();
      //Serial.println(FAILED_COUNTER);
      reset_ethernet();
      init_ethernet();
      sendData();
      lcd.noBlink();
    }
  }

  /*
  while (client.connected()) { // wait 25 seconds for response  
   if (client.available()) {
   char c = client.read();
   Serial.print(c);
   }
   }
   */

  if(client.connected()) {
    
    /** ANTWORT VOM WEBSERVER **/
    unsigned long reqTime = millis();

    // wait for a response and disconnect 
    while ( millis() < reqTime + 25000) { // wait 25 seconds for response  
      if (client.available()) {
        char c = client.read();
        if(c == TIME_HEADER) {
          // Wenn "T" (TimeCode / T<Unix>) gesendet wurde
          Serial.println("++++ Antwort ist ein ZEITSTEMPEL +++");
          time_t pctime = 0;
          for(int i=0; i < TIME_MSG_LEN -1; i++){   
            c = client.read();          
            if( c >= '0' && c <= '9'){   
              pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
            }
          }   
          setTime(pctime);   // Sync Arduino clock to the time received on the serial port
        } 
        else {
          // Wenn keine "T" gesendert wurden
          Serial.print(c);
        }
      }

      /* VERBINDUNG ABBAUEN */
      if (!client.connected()) {
        lcd.noBlink();
        lcd.setCursor(0,3);
        lcd.print("> synced!        ");
        delay(1000);
        count_LCD = 1; //Zähler auf 1 Zurücksetzten damit der komplette Screen neu geschrieben wird und nur nur die "teil" Aktualisierungen
        //Serial.println();
        Serial.println("Server Verbindung getrennt.");
        break;
      }
    }

    client.stop();
    Serial.println("Ethernet Client gestopt.");


    // Wenn keine Fehler auftritt kann counter auf/zurück auf null!
    FAILED_COUNTER=0;

    // Uhrzeit anzeigen
    digitalClockDisplay();
  }

}


// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) { 
      Serial.print("0"); 
    }
    Serial.print(deviceAddress[i], HEX);

  }
}

// function to print a device address
void printAddressForWeb(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) client.print("0");
    client.print(deviceAddress[i], HEX);
  }
}

void print_time(unsigned long inttime, char modus[])
{
  char buffer[20];
  int days, hours, mins, secs, millisec;

  // Millisec
  millisec = inttime % 1000;

  // Time to Sec
  inttime = inttime / 1000;

  // number of days is total number of seconds divided by 24 divided by 3600 !!! ACHTUNG inttime / (24*3600) geht nicht!!!!!!!!!!
  days     = inttime / 86400;
  inttime  = inttime % 86400;

  // Now, inttime is the remainder after subtracting the number of seconds
  // in the number of days
  hours    = inttime / 3600;
  inttime  = inttime % 3600;

  // Now, inttime is the remainder after subtracting the number of seconds
  // in the number of days and hours
  mins     = inttime / 60;
  inttime  = inttime % 60;

  // Now inttime is the number of seconds left after subtracting the number
  // in the number of days, hours and minutes. In other words, it is the
  // number of seconds.
  secs = inttime;

  if(modus == "upload") {
    sprintf(buffer, "%02dm %02ds", mins, secs);
  } 
  else {
    sprintf(buffer, "%03dd %02dh %02dm", days, hours, mins);
  }
  lcd.print(buffer);
}


void loop() {


  Serial.print("LCD Update. Screen Counter: ");
  Serial.println(count_LCD);

  // get Millis
  signed long currentMillis = millis();

  // Send Data block
  if(currentMillis - sendDataMillis > sendDataInterval) {
    sendDataMillis = currentMillis;

    sendData();

    delay(1000);
  }

  // Temperaturen holen alle x Millisekunden

  if(currentMillis - requestTemperaturesMillis > requestTemperaturesInterval) {
    requestTemperaturesMillis = currentMillis;


    //Serial.println("LOCAL.1W: Temperaturen von allen Sensoren abfragen");

    /*
     lcd.clear();
     lcdPrintHeader();
     lcd.setCursor(0,1);
     lcd.print("     OneWireBus");
     lcd.setCursor(0,2);
     lcd.print("> Requesting Temps");
     lcd.setCursor(0,3);
     lcd.print("> waiting...   ");
     
     */
    sensors.requestTemperatures();

    /*
     lcd.setCursor(0,3);
     lcd.print("> done!       ");
     delay(1000);
     
     */


  }

  // Wasserstand
  if (analogRead(1) <= 900) {
    highwater = 1;
  } 
  else {
    highwater = 0;
  }

  // LCD

  if(count_LCD <= 20) {
    // Temp #1
    float Temp_Sensor1 = sensors.getTempC(OneWire_Sensor1);
    float Temp_Sensor2 = sensors.getTempC(OneWire_Sensor2);
    float Temp_Sensor4 = sensors.getTempC(OneWire_Sensor4);

    if(count_LCD == 1) {
      lcd.clear();
      lcdPrintHeader();
      lcd.setCursor(0,1);
      lcd.print("Wasser:");
      lcd.setCursor(0,2);
      lcd.print("Solaranlage:");
      lcd.setCursor(0,3);
      lcd.print("Au");
      lcd.write(byte(0));
      lcd.print("enluft:");
    }
    lcdPrintHeader();
    lcd.setCursor(13,1);
    if(Temp_Sensor1 <= -127.00) {
      lcd.print("n/v");
    } 
    else {
      lcd.print(Temp_Sensor1);
      lcd.write(1);
      lcd.print("C");
    }
    lcd.setCursor(13,2);
    if(Temp_Sensor2 <= -127.00) {
      lcd.print("n/v");
    } 
    else {
      lcd.print(Temp_Sensor2);
      lcd.write(1);
      lcd.print("C");
    }
    lcd.setCursor(13,3);
    if(Temp_Sensor4 <= -127.00) {
      lcd.print("n/v");
    } 
    else {
      lcd.print(Temp_Sensor4);
      lcd.write(1);
      lcd.print("C");
    }
    delay(1000);

    count_LCD++;

  } 
  else if(count_LCD <= 25) { 
    // LCD Screen #2
    float Temp_Sensor3 = sensors.getTempC(OneWire_Sensor3);
    long sendDataMillisRemain = sendDataInterval-(currentMillis-sendDataMillis);
    long sendDataSecRemain = sendDataMillisRemain-(sendDataMillisRemain%1000);

    if(count_LCD == 21) {
      lcd.clear();
      lcdPrintHeader();
      lcd.setCursor(0,1);
      lcd.print("Upload:");
      lcd.setCursor(0,2);
      lcd.print("Uptime:");
      lcd.setCursor(0,3);
      lcd.print("SysTemp:");
    }
    lcdPrintHeader();
    lcd.setCursor(13,1);
    print_time(sendDataSecRemain,"upload");
    lcd.setCursor(8,2);
    print_time(millis(),"uptime");
    lcd.setCursor(13,3);
    if(Temp_Sensor3 <= -127.00) {
      lcd.print("n/v");
    } 
    else {
      lcd.print(Temp_Sensor3);
      lcd.write(1);
      lcd.print("C");
    }
    delay(1000);
    count_LCD++;

  } 
  else {
    // Beginne LCD Loop von vorne...
    //Serial.print("LOCAL.UPTIME: VALUE ");
    //Serial.println(millis());
    count_LCD = 1;
  }

}


