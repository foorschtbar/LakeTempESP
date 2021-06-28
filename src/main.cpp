#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
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
#include <Adafruit_NeoPixel.h>
#include <LittleFS.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include "config.h"

// ++++++++++++++++++++++++++++++++++++++++
//
// CONSTANTS
//
// ++++++++++++++++++++++++++++++++++++++++

// GPIOs
#define PIN_TOUCHSENSOR D0
#define PIN_WATERSENSOR D5
#define PIN_ONE_WIRE_BUS D6
#define PIN_WS2812B D7
#define PIN_LIGHTSENSOR A0

// LCD
#define LCD_COLUMS 20
#define LCD_ROWS 4
#define LCD_SPACE_SYMBOL 0x20 //space symbol from the LCD ROM, see p.9 of GDM2004D datasheet

// Other
#define MAX_NUM_SENSORS 10
#define HOSTNAME "LakeTempESP"

// Constatns
const int SHOW_SCREEN_0_TIME = 30;       // How long screen #1 are shown
const int SHOW_SCREEN_1_TIME = 5;        // How long screen #2 are shown
const int SHOW_SCREEN_2_TIME = 5;        // How long screen #3 are shown
const long SENDDATA_INTERVAL = 300000;   // 5 Min (Regelmaesiger Upload)
const long REQ_SENSOR_INTERVAL = 30000;  // 30 sek
const long TIME_BUTTON_LONGPRESS = 1000; // 1s
const bool HEADER_TIME_WITH_SEC = true;  // Show secconds in header

// Constants - Misc
const char FIRMWARE_VERSION[] = "5.1";
const char COMPILE_DATE[] = __DATE__ " " __TIME__;

//  Eszett
const uint8_t Eszett[8] = {
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
const uint8_t Degree[8] = {
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
const uint8_t Copy[8] = {
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
const uint8_t Heart1[8] = {
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
const uint8_t Heart2[8] = {
    B00000,
    B01010,
    B11111,
    B11111,
    B01110,
    B00100,
    B00000,
    B00000,
};

// ++++++++++++++++++++++++++++++++++++++++
//
// ENUMS
//
// ++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++
//
// LIBS
//
// ++++++++++++++++++++++++++++++++++++++++

// LCD Setup
LiquidCrystal_I2C lcd(PCF8574_ADDR_A20_A10_A00, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

// Setup a oneWire instance to communicate with any OneWire devices (not just MLOCAL >>im/Dallas temperature ICs)
OneWire oneWire(PIN_ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// BME280 Sensor
Adafruit_BME280 bme; // I2C

// LCD Screen Switching
Screens screen(3, SHOW_SCREEN_0_TIME, SHOW_SCREEN_1_TIME, SHOW_SCREEN_2_TIME);

// Webserver
ESP8266WebServer server(80);

// Udpater
ESP8266HTTPUpdateServer httpUpdater;

// NeoPixel library
Adafruit_NeoPixel pixels(1, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

// NTPCLient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);

// WiFi Manager
WiFiManager wifiManager;

// WiFi connection ticker
Ticker ticker;

// ++++++++++++++++++++++++++++++++++++++++
//
// VARS
//
// ++++++++++++++++++++++++++++++++++++++++

// Buffers
String html;
char buff[255];

// Variables will change
uint8_t highwater = 0;
unsigned long lastSendData = 0;                     // erster Upload nach 1er Minute (Damit wird currentMillis unten im loop hochgesetzt ('-' + '-' denn gibt plus!)
unsigned long lastSensorVals = 0;                   // will store last time temps and water was updated
unsigned long buttonTimer = 0;                      // will store how long button was pressed
uint8_t numSensorsFound = 0;                        // number of oneWire sensors
bool previousButtonState = 1;                       // will store last Button state. 1 = unpressed, 0 = pressed
bool lastTimeSyncWasSuccessfull = 0;                // Will store if last sync with NTP server was okay
float tempSensorValues[MAX_NUM_SENSORS];            // will store temp sensor values
DeviceAddress tempSensorAddresses[MAX_NUM_SENSORS]; // will store temp sensor addresses
bool shouldSaveConfig = false;

typedef struct
{
  String apiUrl;
  String apiKey;
  String username = "admin";
  String password = "admin";
  String note;
  int sensorIdxWater = 0;
  int sensorIdxSolar = 0;
  int sensorIdxAir = 0;
} configData_t;
configData_t cfg;

// Colors
#define COLOR_BRIGHTNESS 100
#define COLOR_BLACK pixels.Color(0, 0, 0)
#define COLOR_WIFI pixels.Color(0, 0, COLOR_BRIGHTNESS)
#define COLOR_RED pixels.Color(COLOR_BRIGHTNESS, 0, 0)
#define COLOR_OKAY pixels.Color(0, COLOR_BRIGHTNESS, 0)
#define COLOR_TOUCH pixels.Color(COLOR_BRIGHTNESS, COLOR_BRIGHTNESS, COLOR_BRIGHTNESS)
uint32_t lastLEDStatus = COLOR_BLACK;

void HTMLHeader(const char section[], unsigned int refresh = 0, const char url[] = "/");

// ++++++++++++++++++++++++++++++++++++++++
//
// MAIN CODE
//
// ++++++++++++++++++++++++++++++++++++++++

void showWEBAction()
{
  //setStatus(COLOR_TOUCH);
  // analogWrite(HWPIN_LED_WIFI, 0);
  // ledOneTime = millis();
}

void HTMLHeader(const char *section, unsigned int refresh, const char *url)
{
  char title[50];
  char hostname[50];
  WiFi.hostname().toCharArray(hostname, 50);
  snprintf(title, 50, "LakeTempESP@%s - %s", hostname, section);

  html = "<!DOCTYPE html>";
  html += "<html>\n";
  html += "<head>\n";
  html += "<meta name='viewport' content='width=600' />\n";
  if (refresh != 0)
  {
    html += "<META http-equiv='refresh' content='";
    html += refresh;
    html += ";URL=";
    html += url;
    html += "'>\n";
  }
  html += "<title>";
  html += title;
  html += "</title>\n";
  html += "<style>\n";
  html += "body {\n";
  html += " background-color: #EDEDED;\n";
  html += " font-family: Arial, Helvetica, Sans-Serif;\n";
  html += " Color: #333;\n";
  html += "}\n";
  html += "\n";
  html += "h1 {\n";
  html += "  background-color: #333;\n";
  html += "  display: table-cell;\n";
  html += "  margin: 20px;\n";
  html += "  padding: 20px;\n";
  html += "  color: white;\n";
  html += "  border-radius: 10px 10px 0 0;\n";
  html += "  font-size: 20px;\n";
  html += "}\n";
  html += "\n";
  html += "ul {\n";
  html += "  list-style-type: none;\n";
  html += "  margin: 0;\n";
  html += "  padding: 0;\n";
  html += "  overflow: hidden;\n";
  html += "  background-color: #333;\n";
  html += "  border-radius: 0 10px 10px 10px;";
  html += "}\n";
  html += "\n";
  html += "li {\n";
  html += "  float: left;\n";
  html += "}\n";
  html += "\n";
  html += "li a {\n";
  html += "  display: block;\n";
  html += "  color: #FFF;\n";
  html += "  text-align: center;\n";
  html += "  padding: 16px;\n";
  html += "  text-decoration: none;\n";
  html += "}\n";
  html += "\n";
  html += "li a:hover {\n";
  html += "  background-color: #111;\n";
  html += "}\n";
  html += "\n";
  html += "#main {\n";
  html += "  padding: 20px;\n";
  html += "  background-color: #FFF;\n";
  html += "  border-radius: 10px;\n";
  html += "  margin: 10px 0;\n";
  html += "}\n";
  html += "\n";
  html += "#footer {\n";
  html += "  border-radius: 10px;\n";
  html += "  background-color: #333;\n";
  html += "  padding: 10px;\n";
  html += "  color: #FFF;\n";
  html += "  font-size: 12px;\n";
  html += "  text-align: center;\n";
  html += "}\n";

  html += "table  {\n";
  html += "border-spacing: 0;\n";
  html += "}\n";

  html += "table td, table th {\n";
  html += "padding: 5px;\n";
  html += "}\n";

  html += "table tr:nth-child(even) {\n";
  html += "background: #EDEDED;\n";
  html += "}";

  html += "input[type=\"submit\"] {\n";
  html += "background-color: #333;\n";
  html += "border: none;\n";
  html += "color: white;\n";
  html += "padding: 5px 25px;\n";
  html += "text-align: center;\n";
  html += "text-decoration: none;\n";
  html += "display: inline-block;\n";
  html += "font-size: 16px;\n";
  html += "margin: 4px 2px;\n";
  html += "cursor: pointer;\n";
  html += "}\n";

  html += "input[type=\"submit\"]:hover {\n";
  html += "background-color:#4e4e4e;\n";
  html += "}\n";

  html += "input[type=\"submit\"]:disabled {\n";
  html += "opacity: 0.6;\n";
  html += "cursor: not-allowed;\n";
  html += "}\n";

  html += "</style>\n";
  html += "</head>\n";
  html += "<body>\n";
  html += "<h1>";
  html += title;
  html += "</h1>\n";
  html += "<ul>\n";
  html += "<li><a href='/'>Home</a></li>\n";
  html += "<li><a href='/settings'>Settings</a></li>\n";
  html += "<li><a href='/fwupdate'>FW Update</a></li>\n";
  html += "<li><a href='/reset'>Factory reset</a></li>\n";
  html += "<li><a href='/reboot'>Reboot</a></li>\n";
  html += "</ul>\n";
  html += "<div id='main'>";
}

void HTMLFooter()
{
  html += "</div>";
  html += "<div id='footer'>&copy; 2021 Fabian Otto - Firmware v";
  html += FIRMWARE_VERSION;
  html += " - Compiled at ";
  html += COMPILE_DATE;
  html += "</div>\n";
  html += "</body>\n";
  html += "</html>\n";
}

void SaveConfigCallback()
{
  shouldSaveConfig = true;
}

void SetConfigVars(DynamicJsonDocument &json)
{

  if (json.containsKey("sensorIdxAir"))
  {
    cfg.sensorIdxAir = json["sensorIdxAir"].as<int>();
  }

  if (json.containsKey("sensorIdxSolar"))
  {
    cfg.sensorIdxSolar = json["sensorIdxSolar"].as<int>();
  }

  if (json.containsKey("sensorIdxWater"))
  {
    cfg.sensorIdxWater = json["sensorIdxWater"].as<int>();
  }

  if (json.containsKey("username"))
  {
    cfg.username = json["username"].as<char *>();
  }

  if (json.containsKey("password"))
  {
    cfg.password = json["password"].as<char *>();
  }

  if (json.containsKey("apiKey"))
  {
    cfg.apiKey = json["apiKey"].as<char *>();
  }

  if (json.containsKey("apiUrl"))
  {
    cfg.apiUrl = json["apiUrl"].as<char *>();
  }

  if (json.containsKey("note"))
  {
    cfg.note = json["note"].as<char *>();
  }
}

void SaveConfig()
{
  //save the custom parameters to FS
  if (shouldSaveConfig)
  {
    DynamicJsonDocument json(1024);
    json["apiKey"] = cfg.apiKey;
    json["apiUrl"] = cfg.apiUrl;
    json["note"] = cfg.note;
    json["password"] = cfg.password;
    json["username"] = cfg.username;
    json["sensorIdxAir"] = cfg.sensorIdxAir;
    json["sensorIdxSolar"] = cfg.sensorIdxSolar;
    json["sensorIdxWater"] = cfg.sensorIdxWater;

    File configFile = LittleFS.open("/config.json", "w");
    serializeJson(json, configFile);
    configFile.close();
    Serial.println("[SaveConfig] Saved");
    //end save
  }
}

void LoadConfig()
{
  //file exists, reading and loading
  if (LittleFS.exists("/config.json"))
  {
    File configFile = LittleFS.open("/config.json", "r");

    if (configFile)
    {
      Serial.println("[LoadConfig] Opened config file");

      DynamicJsonDocument json(1024);
      auto error = deserializeJson(json, configFile);

      if (error)
      {
        Serial.print(F("[LoadConfig] deserializeJson() failed with code "));
        Serial.println(error.c_str());
      }
      else
      {
        SetConfigVars(json);
        Serial.println("[LoadConfig] Loaded");
      }
    }
  }
  else
  {
    Serial.println("[LoadConfig] No Configfile, init new file");
    SaveConfigCallback();
    SaveConfig();
  }
}

void resetSettings()
{

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile)
  {
    Serial.println("[resetSettings] Failed to open config file for reset");
  }
  configFile.println("");
  configFile.close();
  wifiManager.resetSettings();
  ESP.restart();
  delay(300);
}

void setStatus(uint32_t c)
{
  pixels.setPixelColor(0, c);
  pixels.show();
}

long dBm2Quality(long dBm)
{
  if (dBm <= -100)
    return 0;
  else if (dBm >= -50)
    return 100;
  else
    return 2 * (dBm + 100);
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
void printAddressForWeb(DeviceAddress deviceAddress, char *buffer, bool upperCase = false)
{
  int8_t off = 0;
  char out[17];
  for (int8_t i = 0; i < 8; i++)
  {
    off += snprintf(out + off, sizeof(out) - off, (upperCase ? "%02X" : "%02x"), deviceAddress[i]);
  }
  strcpy(buffer, out);
}

void initOneWireSensors()
{
  Serial.print(F("[initOneWireSensors] Init sensor arrays..."));

  for (int i = 0; i < MAX_NUM_SENSORS; i++)
  {
    tempSensorValues[i] = DEVICE_DISCONNECTED_C;
    for (int ii = 0; ii < 8; ii++)
    {
      tempSensorAddresses[i][ii] = 0x00;
    }
  }

  Serial.print(F("[initOneWireSensors] Search Sensors..."));

  numSensorsFound = sensors.getDeviceCount();

  Serial.print(F("[initOneWireSensors] Found "));
  Serial.println(numSensorsFound);

  if (numSensorsFound > 0)
  {

    // select the first sensor
    for (int i = 0; i < numSensorsFound; i++)
    {
      Serial.print(F("[initOneWireSensors] #"));
      Serial.print(i);
      Serial.print(F(": "));
      if (sensors.getAddress(tempSensorAddresses[i], i))
      {
        printAddress(tempSensorAddresses[i]);
      }
      else
      {
        Serial.print(F("no address!"));
      }
      Serial.println();
    }
  }
}

void getTemps()
{
  Serial.print("[getTemps] Request temperatures...\n");
  sensors.requestTemperatures();

  for (uint8_t i = 0; i < numSensorsFound; i++)
  {
    tempSensorValues[i] = sensors.getTempC(tempSensorAddresses[i]);
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

  // // Allocate a temporary JsonDocument
  // // Use arduinojson.org/v6/assistant to compute the capacity.
  // StaticJsonDocument<500> jsondoc; //
  //                                  // Add Temps
  // char addressBuffer[16];
  // JsonArray temps = jsondoc.createNestedArray("temps");
  // for (uint8_t i = 0; i < NUM_SENSORS; i++)
  // {
  //   if (tempSensorValues[i] != DEVICE_DISCONNECTED_C)
  //   {
  //     printAddressForWeb(tempSensors[i], addressBuffer);
  //     JsonObject temp = temps.createNestedObject();
  //     temp[addressBuffer] = tempSensorValues[i];
  //   }
  // }
  // JsonArray data = jsondoc.createNestedArray("data");
  // JsonObject jslight = data.createNestedObject();
  // jslight["light"] = analogRead(PIN_LIGHTSENSOR);
  // JsonObject jshighwater = data.createNestedObject();
  // jshighwater["highwater"] = highwater;
  // JsonObject jsuptime = data.createNestedObject();
  // jsuptime["uptime"] = millis();

  // Serial.print(F("[sendData] Sending: "));
  // serializeJson(jsondoc, Serial);
  // Serial.println();

  // char JSONmessageBuffer[500];
  // serializeJson(jsondoc, JSONmessageBuffer, sizeof(JSONmessageBuffer));

  // std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

  // client->setInsecure(); //client->setFingerprint(fingerprint);

  // HTTPClient https;

  // Serial.print("[sendData] HTTPS Client begin...\n");

  // if (https.begin(*client, API_URL))
  // { // HTTPS

  //   https.addHeader("Content-Type:", "application/json"); // Add Header
  //   https.addHeader("X-API-Key:", API_KEY);               // Add Header

  //   Serial.print("[sendData] HTTP POST data...\n");
  //   // start connection and send HTTP header
  //   int httpCode = https.POST(JSONmessageBuffer); // Send the request

  //   // httpCode will be negative on error
  //   if (httpCode > 0)
  //   {
  //     // HTTP header has been send and Server response header has been handled
  //     Serial.printf("[sendData] HTTP Code: %d\n", httpCode);

  //     // file found at server
  //     if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
  //     {
  //       failed = false;

  //       /*Serial.print("[sendData] HTTP Payload: ");
  //       String payload = https.getString();
  //       Serial.println(payload);*/
  //     }
  //   }
  //   else
  //   {
  //     Serial.printf("[sendData] HTTP POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
  //   }

  //   https.end();
  // }
  // else
  // {
  //   Serial.printf("[sendData] HTTP Unable to connect\n");
  // }

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
    lcd.printf("%5.1f", temp);
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
      lcdPrintTemp(tempSensorValues[cfg.sensorIdxWater]);
      lcd.setCursor(13, 2);
      lcdPrintTemp(tempSensorValues[cfg.sensorIdxSolar]);
      lcd.setCursor(13, 3);
      lcdPrintTemp(tempSensorValues[cfg.sensorIdxAir]);
    }
    else if (screen.currentScreen() == 1)
    {
      if (screen.needFullRefresh())
      {
        lcd.clear();
        lcdPrintHeader();
        lcd.setCursor(0, 1);
        lcd.print(F("SysTemp:"));
        lcd.setCursor(0, 2);
        lcd.print(F("Humi:"));
        lcd.setCursor(0, 3);
        lcd.print(F("Press:"));
      }
      lcdPrintHeader();
      lcd.setCursor(14, 1);
      //lcd.print(bme.readTemperature());
      lcd.printf("%4.1f", bme.readTemperature());
      lcd.write(1);
      lcd.print(F("C"));

      lcd.setCursor(14, 2);
      lcd.printf("%5.1f%%", bme.readHumidity());

      lcd.setCursor(9, 3);
      lcd.printf("%7.1f hPa", (bme.readPressure() / 100.0F));
    }
    else if (screen.currentScreen() == 2)
    {
      if (screen.needFullRefresh())
      {
        lcd.clear();
        lcdPrintHeader();
        lcd.setCursor(0, 1);
        lcd.print(F("Uptime:"));
        lcd.setCursor(0, 2);
        lcd.print(F("Upload:"));
        lcd.setCursor(0, 3);
        lcd.print(F("WiFi Signal:"));
      }
      lcdPrintHeader();
      lcd.setCursor(8, 1);
      lcdPrintFormatedTime(millis(), false);

      lcd.setCursor(13, 2);
      lcdPrintFormatedTime((lastSendData + SENDDATA_INTERVAL) - millis(), true);

      lcd.setCursor(16, 3);
      lcd.printf("%3ld%%", dBm2Quality(WiFi.RSSI()));
    }
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
      //setStatus(COLOR_TOUCH);
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
  else
  {
    //setStatus(lastStatus);
  }
  previousButtonState = inp;
}

void tick()
{
  if (lastLEDStatus != COLOR_BLACK)
  {
    setStatus(COLOR_BLACK);
    lastLEDStatus = COLOR_BLACK;
  }
  else
  {
    setStatus(COLOR_WIFI);
    lastLEDStatus = COLOR_WIFI;
  }
}

void EnteredHotspotCallback(WiFiManager *manager)
{
  Serial.println("[EnteredHotspotCallback] No WiFi config! Hotspot enabled");

  ticker.attach(0.2, tick);

  lcd.clear();
  lcdPrintHeader();
  lcd.setCursor(0, 1);
  lcd.print(F("> No WiFi config!"));
  lcd.setCursor(0, 2);
  lcd.print(F("> Hotspot Mode"));
  lcd.setCursor(15, 2);
  lcd.blink();
}

void wifiConnect()
{

  // Begin Wifi
  Serial.print("[wifiConnect] Connecting to WiFi");

  lcd.clear();
  lcdPrintHeader();
  lcd.setCursor(0, 1);
  lcd.print(F("> Connecting to"));
  lcd.setCursor(0, 2);
  lcd.print(F("> WiFi Network..."));
  lcd.blink();

  setStatus(COLOR_WIFI);
  ticker.attach(0.6, tick);

  WiFi.hostname(HOSTNAME);

  // Set config save notify callback
  wifiManager.setSaveConfigCallback(SaveConfigCallback);
  wifiManager.setAPCallback(EnteredHotspotCallback);
  // Config menue timeout 180 seconds.
  //wifiManager.setConfigPortalTimeout(180);

  if ((digitalRead(PIN_TOUCHSENSOR) && !wifiManager.startConfigPortal(HOSTNAME)) || !wifiManager.autoConnect(HOSTNAME))
  {
    Serial.println("[wifiConnect] Wifi failed to connect and hit timeout");
    delay(3000);
    // Reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(5000);
  }

  SaveConfig();

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
  Serial.println("[wifiConnect] WiFi connected!");
  Serial.print("> IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("> Subnetmask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("> GW: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("> DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("> WiFi Diag:\n");
  WiFi.printDiag(Serial);
  Serial.println();
  delay(1000);
  screen.reset();

  setStatus(COLOR_BLACK);
  ticker.detach();
}

void handleFWUpdate()
{
  showWEBAction();
  if (!server.authenticate(cfg.username.c_str(), cfg.password.c_str()))
  {
    return server.requestAuthentication();
  }
  else
  {
    HTMLHeader("Firmware Update");

    html += "<form method='POST' action='/dofwupdate' enctype='multipart/form-data'>\n";
    html += "<table>\n";
    html += "<tr>\n";
    html += "<td>Current version</td>\n";
    html += String("<td>") + FIRMWARE_VERSION + String("</td>\n");
    html += "</tr>\n";
    html += "<tr>\n";
    html += "<td>Compiled</td>\n";
    html += String("<td>") + COMPILE_DATE + String("</td>\n");
    html += "</tr>\n";
    html += "<tr>\n";
    html += "<td>Firmware file</td>\n";
    html += "<td><input type='file' accept='.bin, .bin.gz' name='update'></td>\n";
    html += "</tr>\n";
    html += "</table>\n";
    html += "<br />";
    html += "<input type='submit' value='Update'>";
    html += "</form>";
    HTMLFooter();
    server.send(200, "text/html", html);
  }
}

void handleReboot()
{
  showWEBAction();
  if (!server.authenticate(cfg.username.c_str(), cfg.password.c_str()))
  {
    return server.requestAuthentication();
  }
  else
  {
    boolean reboot = false;
    if (server.method() == HTTP_POST)
    {

      HTMLHeader("Reboot", 10, "/");
      html += "Reboot in progress...";
      reboot = true;
    }
    else
    {
      HTMLHeader("Reboot");
      html += "<form method='POST' action='/reboot'>";
      html += "<input type='submit' value='Reboot'>";
      html += "</form>";
    }
    HTMLFooter();

    server.send(200, "text/html", html);

    if (reboot)
    {
      delay(200);
      ESP.reset();
      delay(300);
    }
  }
}

void handleReset()
{
  showWEBAction();
  if (!server.authenticate(cfg.username.c_str(), cfg.password.c_str()))
  {
    return server.requestAuthentication();
  }
  else
  {
    boolean reset = false;
    if (server.method() == HTTP_POST)
    {
      HTMLHeader("Reboot", 10, "/");
      html += "Settings deleted! Reboot in progress...";
      reset = true;
    }
    else
    {
      HTMLHeader("Reboot");
      html += "<form method='POST' action='/reset'>";
      html += "<input type='submit' value='Reset settings'>";
      html += "</form>";
    }
    HTMLFooter();

    server.send(200, "text/html", html);

    if (reset)
    {
      delay(200);
      resetSettings();
    }
  }
}

void printSensorOptionList(uint8_t selectedValue)
{
  char buffer[50];

  for (uint8_t i = 0; i < numSensorsFound; i++)
  {
    printAddressForWeb(tempSensorAddresses[i], buffer, true);

    html += "<option value='";
    html += i;
    html += (selectedValue == i ? "' selected>" : "'>");
    html += buffer;
    html += "</option>";
  }
}

void handleSettings()
{
  showWEBAction();
  if (!server.authenticate(cfg.username.c_str(), cfg.password.c_str()))
  {
    return server.requestAuthentication();
  }
  else
  {
    boolean saveandreboot = false;
    String value;
    if (server.method() == HTTP_POST)
    { // Save Settings

      for (uint8_t i = 0; i < server.args(); i++)
      {
        // Trim String
        value = server.arg(i);
        value.trim();

        if (server.argName(i) == "apiKey")
        {
          cfg.apiKey = value;
        }
        else if (server.argName(i) == "apiUrl")
        {
          cfg.apiUrl = value;
        }
        else if (server.argName(i) == "note")
        {
          cfg.note = value;
        }
        else if (server.argName(i) == "username")
        {
          cfg.username = value;
        }
        else if (server.argName(i) == "password")
        {
          cfg.password = value;
        }
        else if (server.argName(i) == "sensorIdxWater")
        {
          cfg.sensorIdxWater = value.toInt();
        }
        else if (server.argName(i) == "sensorIdxSolar")
        {
          cfg.sensorIdxSolar = value.toInt();
        }
        else if (server.argName(i) == "sensorIdxAir")
        {
          cfg.sensorIdxAir = value.toInt();
        }
        saveandreboot = true;
      }
    }

    if (saveandreboot)
    {
      HTMLHeader("Settings", 10, "/settings");
      html += "New Settings saved! Device will be reboot...";
    }
    else
    {
      HTMLHeader("Settings");
      html += "<form action='/settings' method='post'>\n";
      html += "<table>\n";

      html += "<tr>\n<td>API URL:</td>\n";
      html += "<td><input name='apiUrl' type='text' autocapitalize='none' value='";
      html += cfg.apiUrl;
      html += "'></td></tr>\n";

      html += "<tr>\n<td>API Key:</td>\n";
      html += "<td><input name='apiKey' type='text' autocapitalize='none' value='";
      html += cfg.apiKey;
      html += "'></td></tr>\n";

      html += "<tr>\n<td>Note:</td>\n";
      html += "<td><input name='note' type='text' autocapitalize='none' value='";
      html += cfg.note;
      html += "'></td></tr>\n";

      html += "<tr>\n<td>\nUsername:</td>\n";
      html += "<td><input name='username' type='text' maxlength='30' autocapitalize='none' value='";
      html += cfg.username;
      html += "'></td>\n</tr>\n";

      html += "<tr>\n<td>\nPassword:</td>\n";
      html += "<td><input name='password' type='password' maxlength='30' value='";
      html += cfg.password;
      html += "'></td>\n</tr>\n";

      html += "<tr>\n<td>LCD Sensor Wasser:</td>\n";
      html += "<td><select name='sensorIdxWater'>";
      printSensorOptionList(cfg.sensorIdxWater);
      html += "</select>";
      html += "</td>\n</tr>\n";

      html += "<tr>\n<td>LCD Sensor Solar:</td>\n";
      html += "<td><select name='sensorIdxSolar'>";
      printSensorOptionList(cfg.sensorIdxSolar);
      html += "</select>";
      html += "</td>\n</tr>\n";

      html += "<tr>\n<td>LCD Sensor Luft:</td>\n";
      html += "<td><select name='sensorIdxAir'>";
      printSensorOptionList(cfg.sensorIdxAir);
      html += "</select>";
      html += "</td>\n</tr>\n";

      html += "</table>\n";

      html += "<br />\n";
      html += "<input type='submit' value='Save'>\n";
      html += "</form>\n";
    }
    HTMLFooter();
    server.send(200, "text/html", html);

    if (saveandreboot)
    {
      delay(200);
      SaveConfigCallback();
      SaveConfig();
      delay(500);
      ESP.reset();
    }
  }
}

void handleRoot()
{
  Serial.println("[handleRoot] Webserver access");
  showWEBAction();
  HTMLHeader("Main");

  html += "<table>\n";

  char timebuf[20];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int days = hr / 24;
  snprintf(timebuf, 20, " %02d:%02d:%02d:%02d", days, hr % 24, min % 60, sec % 60);

  html += "<tr>\n<td>Uptime:</td>\n<td>";
  html += timebuf;
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>Current time:</td>\n<td>";
  html += timeClient.getFormattedDate();
  html += " (UTC)</td>\n</tr>\n";

  html += "<tr>\n<td>Firmware:</td>\n<td>v";
  html += FIRMWARE_VERSION;
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>Compiled:</td>\n<td>";
  html += COMPILE_DATE;
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>OneWire Sensors:</td>\n<td>";
  for (uint8_t i = 0; i < numSensorsFound; i++)
  {

    snprintf(buff, 50, "%d/%d: ", i + 1, numSensorsFound);
    html += buff;
    html += " <code>";
    printAddressForWeb(tempSensorAddresses[i], buff, true);
    html += buff;
    html += "</code> (";
    snprintf(buff, 50, "%.1f", tempSensorValues[i]);
    html += buff;
    html += " &deg;C)<br />";
  }
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>BME Sensor:</td>\n<td>";
  html += "Temperature: ";
  html += bme.readTemperature();
  html += " &deg;C<br />";
  html += "Pressure: ";
  html += (bme.readPressure() / 100.0F);
  html += " hPa<br />";
  html += "Humidity: ";
  html += bme.readHumidity();
  html += " %";

  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>Note:</td>\n<td>";
  if (strcmp(cfg.note.c_str(), "") == 0)
  {
    html += "---";
  }
  else
  {
    html += cfg.note;
  }
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>Hostname:</td>\n<td>";
  html += WiFi.hostname().c_str();
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>IP address:</td>\n<td>";
  html += WiFi.localIP().toString();
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>Subnetmask:</td>\n<td>";
  html += WiFi.subnetMask().toString();
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>Gateway:</td>\n<td>";
  html += WiFi.gatewayIP().toString();
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>DNS server:</td>\n<td>";
  html += WiFi.dnsIP().toString();
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>MAC address:</td>\n<td>";
  html += WiFi.macAddress().c_str();
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>Signal strength:</td>\n<td>";
  html += dBm2Quality(WiFi.RSSI());
  html += "% (";
  html += WiFi.RSSI();
  html += " dBm)</td>\n</tr>\n";

  html += "<tr>\n<td>Client IP:</td>\n<td>";
  html += server.client().remoteIP().toString().c_str();
  html += "</td>\n</tr>\n";

  html += "</table>\n";

  HTMLFooter();
  server.send(200, "text/html", html);
}

void handleNotFound()
{
  showWEBAction();
  HTMLHeader("File Not Found");
  html += "URI: ";
  html += server.uri();
  html += "<br />\nMethod: ";
  html += (server.method() == HTTP_GET) ? "GET" : "POST";
  html += "<br />\nArguments: ";
  html += server.args();
  html += "<br />\n";
  HTMLFooter();
  for (uint8_t i = 0; i < server.args(); i++)
  {
    html += " " + server.argName(i) + ": " + server.arg(i) + "<br />\n";
  }

  server.send(404, "text/html", html);
}

void setup(void)
{

  Serial.begin(115200);
  Serial.print(F("\n\n== Welcome to LakeTemp "));
  Serial.print(FIRMWARE_VERSION);
  Serial.println(F(" =="));

  // Mounting FileSystem
  Serial.println(F("[setup] Mounting file system..."));
  if (LittleFS.begin())
  {
    Serial.println(F("[setup] Mounted file system."));
    LoadConfig();
  }
  else
  {
    Serial.println(F("[setup] Failed to mount FS"));
  }

  // Setting the I/O pin modes
  pinMode(PIN_TOUCHSENSOR, INPUT);
  pinMode(PIN_WATERSENSOR, INPUT);

  // NeoPixel
  pixels.begin();
  pixels.clear();
  setStatus(COLOR_RED);

  // BME Begin
  Serial.println("[setup] Searching BME280 sensor...");
  if (!bme.begin(BME280_ADDRESS_ALTERNATE))
  {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x");
    Serial.println(bme.sensorID(), 16);
  }
  else
  {
    Serial.println("[setup] Found valid BME280 sensor!");
    Serial.print("> Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("> Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("> Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
  }

  // OneWire
  Serial.println("[setup] Searching OneWire sensors...");
  sensors.begin();
  initOneWireSensors();
  getTemps();

  // LCD begin
  while (lcd.begin(LCD_COLUMS, LCD_ROWS) != 1)
  {
    Serial.println(F("[setup] PCF8574 is not connected or lcd pins declaration is wrong. Only pins numbers: 4,5,6,16,11,12,13,14 are legal."));
    delay(5000);
  }

  Serial.println(F("[setup] PCF8574 connected!"));

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

  // Wifi connect
  wifiConnect();

  // NTPCLient
  timeClient.begin();

  // Arduino OTA Update
  httpUpdater.setup(&server, "/dofwupdate", cfg.username, cfg.password);

  // Webserver
  server.on("/", handleRoot);
  server.on("/reset", handleReset);
  server.on("/reboot", handleReboot);
  server.on("/settings", handleSettings);
  server.on("/fwupdate", handleFWUpdate);
  server.onNotFound(handleNotFound);
  server.begin();

  // Setup complete
  setStatus(COLOR_OKAY);
}

void loop()
{
  // NTPCLient
  lastTimeSyncWasSuccessfull = timeClient.update();

  // handleButton
  handleButton();

  // Webserver
  server.handleClient();

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
