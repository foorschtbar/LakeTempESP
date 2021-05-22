#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <ArduinoJson.h>
#include <DallasTemperature.h> // http://milesburton.com/index.php/Dallas_Temperature_Control_Library
#include <Screens.h>
#include <NTPClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "config.h"

// GPIOs
#define PIN_TOUCHSENSOR D0
#define PIN_WATERSENSOR D5
#define PIN_ONE_WIRE_BUS D6
#define PIN_LIGHTSENSOR A0

// LCD
#define LCD_COLUMS 20
#define LCD_ROWS 4
#define LCD_SPACE_SYMBOL 0x20 //space symbol from the LCD ROM, see p.9 of GDM2004D datasheet

// Constatns
const int SHOW_SCREEN_0_TIME = 30;       // How long screen #1 are shown
const int SHOW_SCREEN_1_TIME = 5;        // How long screen #2 are shown
const int SHOW_SCREEN_2_TIME = 5;        // How long screen #3 are shown
const long SENDDATA_INTERVAL = 300000;   // 5 Min (Regelmaesiger Upload)
const long REQ_SENSOR_INTERVAL = 30000;  // 30 sek
const long TIME_BUTTON_LONGPRESS = 1000; // 1s
const bool HEADER_TIME_WITH_SEC = true;  // Show secconds in header

// Constants - Misc
const char FIRMWARE_VERSION[] = "5.0";
const char COMPILE_DATE[] = __DATE__ " " __TIME__;

//  Eszett
uint8_t Eszett[8] = {
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
uint8_t Degree[8] = {
    B01100,
    B10010,
    B10010,
    B01100,
    B00000,
    B00000,
    B00000,
    B00000,
};

// Copy
uint8_t Copy[8] = {
    B01110,
    B10001,
    B10111,
    B11001,
    B11001,
    B10111,
    B10001,
    B01110,
};

// Heart1
uint8_t Heart1[8] = {
    B00000,
    B01010,
    B10101,
    B10001,
    B01010,
    B00100,
    B00000,
    B00000,
};

// Heart2
uint8_t Heart2[8] = {
    B00000,
    B01010,
    B11111,
    B11111,
    B01110,
    B00100,
    B00000,
    B00000,
};
// Variables will change
uint8_t highwater = 0;
unsigned long lastSendData = 0;      // erster Upload nach 1er Minute (Damit wird currentMillis unten im loop hochgesetzt ('-' + '-' denn gibt plus!)
unsigned long lastSensorVals = 0;    // will store last time temps and water was updated
unsigned long buttonTimer = 0;       // will store how long button was pressed
uint8_t numSensorsFound = 0;         // number of oneWire sensors
bool previousButtonState = 1;        // will store last Button state. 1 = unpressed, 0 = pressed
bool lastTimeSyncWasSuccessfull = 0; // Will store if last sync with NTP server was okay
float tempSensorValues[NUM_SENSORS];

// LCD Setup
LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

// Setup a oneWire instance to communicate with any OneWire devices (not just MLOCAL >>im/Dallas temperature ICs)
OneWire oneWire(PIN_ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// BME280 Sensor
Adafruit_BME280 bme; // I2C

// LCD Screen Switching
Screens screen(3, SHOW_SCREEN_0_TIME, SHOW_SCREEN_1_TIME, SHOW_SCREEN_2_TIME);

// NTPCLient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);

long RSSI2Quality(long dBm)
{
  if (dBm <= -100)
    return 0;
  else if (dBm >= -50)
    return 100;
  else
    return 2 * (dBm + 100);
}

void initTemps()
{
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    tempSensorValues[i] = DEVICE_DISCONNECTED_C;
  }
}

void getTemps()
{
  Serial.print("[getTemps] Request temperatures...\n");
  sensors.requestTemperatures();

  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    tempSensorValues[i] = sensors.getTempC(tempSensors[i]);
    if (tempSensorValues[i] == DEVICE_DISCONNECTED_C)
    {
      Serial.printf("[getTemps] #%i: disconnected\n", i);
    }
    else
    {
      Serial.printf("[getTemps] #%i: %.01f °C\n", i, tempSensorValues[i]);
    }
  }
}

void printDigits(int digits)
{
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(F(":"));
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void lcdPrintFormatedTime(unsigned long inttime, boolean shorttime)
{

  char buffer[20];
  int days, hours, mins, secs;

  // Time to Sec
  inttime = inttime / 1000;

  // number of days is total number of seconds divided by 24 divided by 3600 !!! ACHTUNG inttime / (24*3600) geht nicht!!!!!!!!!!
  days = inttime / 86400;
  inttime = inttime % 86400;

  // Now, inttime is the remainder after subtracting the number of seconds
  // in the number of days
  hours = inttime / 3600;
  inttime = inttime % 3600;

  // Now, inttime is the remainder after subtracting the number of seconds
  // in the number of days and hours
  mins = inttime / 60;
  inttime = inttime % 60;

  // Now inttime is the number of seconds left after subtracting the number
  // in the number of days, hours and minutes. In other words, it is the
  // number of seconds.
  secs = inttime;

  if (shorttime)
  {
    snprintf_P(buffer, sizeof(buffer), PSTR("%02dm %02ds"), mins, secs);
  }
  else
  {
    if (days == 0)
    {
      snprintf_P(buffer, sizeof(buffer), PSTR(" %02dh %02dm %02ds"), hours, mins, secs);
    }
    else
    {
      snprintf_P(buffer, sizeof(buffer), PSTR("%03dd %02dh %02dm"), days, hours, mins);
    }
  }

  lcd.print(buffer);
}

void lcdPrintHeader()
{
  char buffer[10];
  lcd.setCursor(0, 0);
  if (screen.currentTick())
  {
    lcd.write(3);
  }
  else
  {
    lcd.write(4);
  }
  lcd.setCursor(1, 0);
  lcd.print(F("LakeTemp"));
  lcd.setCursor(11, 0);
  char tick = ':';

  if (screen.currentTick())
    tick = ' ';

  if (lastTimeSyncWasSuccessfull)
  {
    if (HEADER_TIME_WITH_SEC)
    {
      snprintf_P(buffer, sizeof(buffer), PSTR(" %02d%c%02d%c%02d"), timeClient.getHours(), tick, timeClient.getMinutes(), tick, timeClient.getSeconds());
    }
    else
    {
      snprintf_P(buffer, sizeof(buffer), PSTR("%02d%c%02d Uhr"), timeClient.getHours(), tick, timeClient.getSeconds());
    }
  }
  else
  {
    if (HEADER_TIME_WITH_SEC)
    {
      snprintf_P(buffer, sizeof(buffer), PSTR(" --%c--%c--"), tick, tick);
    }
    else
    {
      snprintf_P(buffer, sizeof(buffer), PSTR("--%c-- Uhr"), tick);
    }
  }
  lcd.print(buffer);
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
    {
      Serial.print(F("0"));
    }
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print a device address
void printAddressForWeb(DeviceAddress deviceAddress, char *buffer)
{
  int8_t off = 0;
  char out[16];
  for (int8_t i = 0; i < 8; i++)
  {
    off += snprintf(out + off, sizeof(out) - off, "%02x", deviceAddress[i]);
  }
  strcpy(buffer, out);
}

void sendData()
{
  bool failed = true;

  Serial.println(F("[sendData] Start upload...")); //
                                                   // Send the command to get temperatures
  getTemps();                                      //
  lcd.clear();
  lcdPrintHeader();
  lcd.setCursor(0, 1);
  lcd.print(F("> Upload data"));
  lcd.setCursor(0, 2);
  lcd.print(F("> syncing..."));
  lcd.blink();

  // Allocate a temporary JsonDocument
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<500> jsondoc; //
                                   // Add Temps
  char addressBuffer[16];
  JsonArray temps = jsondoc.createNestedArray("temps");
  for (uint8_t i = 0; i < NUM_SENSORS; i++)
  {
    if (tempSensorValues[i] != DEVICE_DISCONNECTED_C)
    {
      printAddressForWeb(tempSensors[i], addressBuffer);
      JsonObject temp = temps.createNestedObject();
      temp[addressBuffer] = tempSensorValues[i];
    }
  }
  JsonArray data = jsondoc.createNestedArray("data");
  JsonObject jslight = data.createNestedObject();
  jslight["light"] = analogRead(PIN_LIGHTSENSOR);
  JsonObject jshighwater = data.createNestedObject();
  jshighwater["highwater"] = highwater;
  JsonObject jsuptime = data.createNestedObject();
  jsuptime["uptime"] = millis();

  Serial.print(F("[sendData] Sending: "));
  serializeJson(jsondoc, Serial);
  Serial.println();

  char JSONmessageBuffer[500];
  serializeJson(jsondoc, JSONmessageBuffer, sizeof(JSONmessageBuffer));

  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

  client->setInsecure(); //client->setFingerprint(fingerprint);

  HTTPClient https;

  Serial.print("[sendData] HTTPS Client begin...\n");

  if (https.begin(*client, API_URL))
  { // HTTPS

    https.addHeader("Content-Type:", "application/json"); // Add Header
    https.addHeader("X-API-Key:", API_KEY);               // Add Header

    Serial.print("[sendData] HTTP POST data...\n");
    // start connection and send HTTP header
    int httpCode = https.POST(JSONmessageBuffer); // Send the request

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[sendData] HTTP Code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        failed = false;

        /*Serial.print("[sendData] HTTP Payload: ");
        String payload = https.getString();
        Serial.println(payload);*/
      }
    }
    else
    {
      Serial.printf("[sendData] HTTP POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  }
  else
  {
    Serial.printf("[sendData] HTTP Unable to connect\n");
  }

  if (!failed)
  {
    lcd.noBlink();
    lcd.setCursor(0, 2);
    lcd.print(F("> synced!        "));
    delay(1000);
    screen.reset();
  }
  else
  {
    lcd.noBlink();
    lcd.setCursor(0, 2);
    lcd.print(F("> failed!        "));
    delay(1000);
    screen.reset();
  }
}

void lcdPrintTemp(float temp)
{
  if (temp == DEVICE_DISCONNECTED_C)
  {
    lcd.print(F("  ---"));
    lcd.write(1);
    lcd.print(F("C"));
  }
  else
  {
    lcd.print(temp);
    lcd.write(1);
    lcd.print(F("C"));
  }
}

void lcdShow()
{
  if (screen.needRefresh() || screen.needFullRefresh())
  {
    //screen.debug(Serial);
    if (screen.currentScreen() == 0)
    {

      if (screen.needFullRefresh())
      {
        lcd.clear();
        lcdPrintHeader();
        lcd.setCursor(0, 1);
        lcd.print(F("Wasser:"));
        lcd.setCursor(0, 2);
        lcd.print(F("Solaranlage:"));
        lcd.setCursor(0, 3);
        lcd.print(F("Au"));
        lcd.write(0);
        lcd.print(F("enluft:"));
      }
      lcdPrintHeader();
      lcd.setCursor(13, 1);
      lcdPrintTemp(tempSensorValues[0]);
      lcd.setCursor(13, 2);
      lcdPrintTemp(tempSensorValues[1]);
      lcd.setCursor(13, 3);
      lcdPrintTemp(tempSensorValues[3]);
    }
    else if (screen.currentScreen() == 1)
    {
      if (screen.needFullRefresh())
      {
        lcd.clear();
        lcdPrintHeader();
        lcd.setCursor(0, 1);
        lcd.print(F("Sensors:"));
        lcd.setCursor(0, 2);
        lcd.print(F("Uptime:"));
        lcd.setCursor(0, 3);
        lcd.print(F("SysTemp:"));
      }
      lcdPrintHeader();
      lcd.setCursor(15, 1);
      lcd.printf("%02d/%02d", numSensorsFound, NUM_SENSORS);
      lcd.setCursor(8, 2);
      lcdPrintFormatedTime(millis(), false);
      lcd.setCursor(13, 3);
      lcdPrintTemp(tempSensorValues[4]);
    }
    else if (screen.currentScreen() == 2)
    {
      if (screen.needFullRefresh())
      {
        lcd.clear();
        lcdPrintHeader();
        lcd.setCursor(0, 1);
        lcd.print(F("Upload:"));
        lcd.setCursor(0, 2);
        lcd.print(F("WiFi Signal:"));
        lcd.setCursor(0, 3);
        lcd.print(F("IP:"));
      }
      lcdPrintHeader();
      lcd.setCursor(13, 1);
      lcdPrintFormatedTime((lastSendData + SENDDATA_INTERVAL) - millis(), true);
      lcd.setCursor(16, 2);
      lcd.printf("%3ld%%", RSSI2Quality(WiFi.RSSI()));
      lcd.setCursor((20 - WiFi.localIP().toString().length()), 3);
      lcd.print(WiFi.localIP());
    }
  }
}

void listSensors()
{
  Serial.print(F("[listSensors] Search Sensors..."));

  numSensorsFound = sensors.getDeviceCount();

  Serial.print(F("found "));
  Serial.println(numSensorsFound);

  // select the first sensor
  for (int i = 0; i < numSensorsFound; i++)
  {
    Serial.print(F("[listSensors] #"));
    Serial.print(i);
    Serial.print(F(": "));
    DeviceAddress deviceAddress;
    if (sensors.getAddress(deviceAddress, i))
    {
      printAddress(deviceAddress);
    }
    else
    {
      Serial.print(F("no adress!"));
    }
    Serial.println();
  }
}

void handleButton()
{
  bool inp = !digitalRead(PIN_TOUCHSENSOR);
  if (inp == 0)
  {
    if (inp != previousButtonState)
    {
      Serial.print("[handleButton] Button pressed short\n");
      buttonTimer = millis();
      screen.nextScreen();
      Serial.printf("[handleButton] Current screen: %i\n", screen.currentScreen());
    }
    if ((millis() - buttonTimer >= TIME_BUTTON_LONGPRESS))
    {
      Serial.print("[handleButton] Button pressed long\n");
      sendData();
    }

    // Delay a little bit to avoid bouncing
    delay(50);
  }
  previousButtonState = inp;
}

void wifiConnect()
{
  lcd.clear();
  lcdPrintHeader();
  lcd.setCursor(0, 1);
  lcd.print(F("> Connecting to"));
  lcd.setCursor(0, 2);
  lcd.print(F("> WiFi Network..."));
  lcd.blink();

  // Begin Wifi
  Serial.print("[wifiConnect] Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.hostname("LakeTempESP");
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");

    // handleButton
    handleButton();
  }
  lcd.noBlink();
  lcd.clear();
  lcdPrintHeader();
  lcd.setCursor(0, 1);
  lcd.print(F("WiFi connected!"));
  lcd.setCursor(0, 2);
  lcd.print(F("IP: "));
  lcd.setCursor(4, 2);
  lcd.print(WiFi.localIP());
  lcd.setCursor(0, 4);
  lcd.print(F("GW: "));
  lcd.setCursor(4, 4);
  lcd.print(WiFi.gatewayIP());

  Serial.println();
  Serial.print("[wifiConnect] WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("[wifiConnect] WiFi Diag:\n");
  WiFi.printDiag(Serial);
  Serial.println();
  delay(1000);
  screen.reset();
}

void setup(void)
{

  Serial.begin(115200);
  Serial.print(F("== Welcome to LakeTemp "));
  Serial.print(FIRMWARE_VERSION);
  Serial.println(F(" =="));

  // Set Temps to start value (-127);
  initTemps();

  // Setting the I/O pin modes
  pinMode(PIN_TOUCHSENSOR, INPUT);
  pinMode(PIN_WATERSENSOR, INPUT);

  // BME Begin
  // status = bme.begin(0x76, &Wire2)
  if (!bme.begin(BME280_ADDRESS_ALTERNATE))
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
  }
  else
  {
    Serial.println("Found valid BME280 sensor!");
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
  }

  // LCD begin
  while (lcd.begin(LCD_COLUMS, LCD_ROWS) != 1)
  {
    Serial.println(F("[setup] PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);
  }

  lcd.createChar(0, Eszett);
  lcd.createChar(1, Degree);
  lcd.createChar(2, Copy);
  lcd.createChar(3, Heart1);
  lcd.createChar(4, Heart2);

  // LCD Info start
  lcd.clear();
  lcd.home();
  lcd.print(F("      LakeTemp   "));
  lcd.setCursor(0, 1);
  lcd.print(F("     Version "));
  lcd.print(FIRMWARE_VERSION);

  lcd.setCursor(0, 2);
  lcd.print(COMPILE_DATE);

  lcd.setCursor(0, 3);
  lcd.print(F("   "));
  lcd.write(2);
  lcd.print(F(" Fabian Otto "));

  lcd.backlight();
  delay(500);
  lcd.noBacklight();
  delay(500);
  lcd.backlight();
  delay(500);
  lcd.noBacklight();
  delay(500);
  lcd.backlight();
  delay(500);
  lcd.noBacklight();
  delay(500);
  lcd.backlight();
  delay(500);
  lcd.noBacklight();
  delay(500);
  lcd.backlight();

  delay(2000);

  // Wifi connect
  wifiConnect();

  // NTPCLient
  timeClient.begin();

  // tempSensors
  sensors.begin();
  listSensors();
}

void loop()
{
  // NTPCLient
  lastTimeSyncWasSuccessfull = timeClient.update();

  // handleButton
  handleButton();

  // Get temperatures from one wire sensors and waterlevel every x seconds
  if (millis() - lastSensorVals > REQ_SENSOR_INTERVAL || lastSensorVals == 0)
  {
    // Temps
    getTemps();

    // Water
    if (digitalRead(PIN_WATERSENSOR))
    {
      highwater = 1;
    }
    else
    {
      highwater = 0;
    }

    lastSensorVals = millis();
  }

  // Send Data block
  if (millis() - lastSendData > SENDDATA_INTERVAL || lastSendData == 0)
  {
    sendData();
    lastSendData = millis();
  }

  screen.loop();

  // LCD
  lcdShow();
}
