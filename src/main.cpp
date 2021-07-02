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
#include "misc.h"

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
#define MAX_NUM_SENSORS 10         //
#define RESET_COUNTDOWN 5          // How long the button needs to be pressed at startup to reset to factory defaults. Warning: Touchsensors goes to low after approx 5s continous pressing!
#define REFRESH_TIME 20            // After this time in sec pages refreshes
#define LED_BLINK_INTERVAL 200     //
#define LED_FADE_INTERVAL 100      //
#define SHOW_SCREEN_0_TIME 30      // How long screen #1 are shown
#define SHOW_SCREEN_1_TIME 5       // How long screen #2 are shown
#define SHOW_SCREEN_2_TIME 5       // How long screen #3 are shown
#define SENDDATA_INTERVAL 300000   // 5 Min (Regelmaesiger Upload)
#define REQ_SENSOR_INTERVAL 30000  // 30 sek
#define TIME_BUTTON_LONGPRESS 1000 // 1s
#define HEADER_TIME_WITH_SEC true  // Show secconds in header

// Constants - Misc
const char FIRMWARE_VERSION[] = "5.1";
const char COMPILE_DATE[] = __DATE__ " " __TIME__;

// ++++++++++++++++++++++++++++++++++++++++
//
// ENUMS
//
// ++++++++++++++++++++++++++++++++++++++++

enum class UploadStatus
{
  Unknown,
  Success,
  Failed
};

enum class WiFiStatus
{
  Unknown,
  Hotspot,
  Client
};

enum class LEDMode
{
  Normal,
  Blink,
  Fade
};

// ++++++++++++++++++++++++++++++++++++++++
//
// LIBS
//
// ++++++++++++++++++++++++++++++++++++++++

// LCD Setup
LiquidCrystal_I2C lcd(PCF8574_ADDR_A20_A10_A00, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE); // Only pins numbers: 4,5,6,16,11,12,13,14 are legal.

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
NTPClient timeClient(ntpUDP);

// ++++++++++++++++++++++++++++++++++++++++
//
// VARS
//
// ++++++++++++++++++++++++++++++++++++++++

// Buffers
String html;
char buff[255];

// Variables will change
uint8_t highwater = 0;                              //
bool forceSendData = true;                          //
unsigned long lastSendData = 0;                     //
unsigned long lastSensorVals = 0;                   // will store last time temps and water was updated
unsigned long buttonTimer = 0;                      // will store how long button was pressed
unsigned long ledTimer = 0;                         //
uint8_t numSensorsFound = 0;                        // number of oneWire sensors
bool lastTimeSyncWasSuccessfull = 0;                // Will store if last sync with NTP server was okay
float tempSensorValues[MAX_NUM_SENSORS];            // will store temp sensor values
DeviceAddress tempSensorAddresses[MAX_NUM_SENSORS]; // will store temp sensor addresses
UploadStatus uploadStatus = UploadStatus::Unknown;  //
WiFiStatus wifiStatus = WiFiStatus::Unknown;        //

typedef struct
{
  String hostname = "LakeTempESP";
  String ssid = "";
  String psk = "";
  String apiUrl = "";
  String apiKey = "";
  String username = "admin";
  String password = "admin";
  String ntpserver = "europe.pool.ntp.org";
  String note = "";
  int sensorIdxWater = 0;
  int sensorIdxSolar = 0;
  int sensorIdxAir = 0;
  int timeoffset = 7200;
} configData_t;
configData_t cfg;

// Colors
#define LED_BRIGHTNESS 100
#define COLOR_BLACK pixels.Color(0, 0, 0)
#define COLOR_BLUE pixels.Color(0, 0, 255)
#define COLOR_RED pixels.Color(255, 0, 0)
#define COLOR_GREEN pixels.Color(0, 255, 0)
#define COLOR_ORANGE pixels.Color(255, 200, 0)
#define COLOR_YELLOW pixels.Color(255, 255, 0)
#define COLOR_TOUCH pixels.Color(255, 255, 255)
uint32_t ledColor = COLOR_BLACK;
bool ledFirstLoop = true;
LEDMode ledMode = LEDMode::Normal;

void HTMLHeader(const char section[], unsigned int refresh = 0, const char url[] = "/");

// ++++++++++++++++++++++++++++++++++++++++
//
// MAIN CODE
//
// ++++++++++++++++++++++++++++++++++++++++

template <typename... Args>
void logf(const char *f, Args... args)
{
#ifdef DEBUG
  Serial.printf(f, args...);
#endif
}

String getUploadStatus()
{
  if (uploadStatus == UploadStatus::Success)
  {
    return "Success";
  }
  else if (uploadStatus == UploadStatus::Failed)
  {
    return "Failed";
  }
  else
  {
    return "Unknown";
  }
}

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

  html += "code {\n";
  html += "font-family : courier, courier new, serif;\n";
  html += "font-size: large;\n";
  html += "}";

  html += "input[type=\"submit\"], button {\n";
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

  html += "input[type=\"submit\"]:hover, button:hover {\n";
  html += "background-color:#4e4e4e;\n";
  html += "}\n";

  html += "input[type=\"submit\"]:disabled, button:disabled {\n";
  html += "opacity: 0.6;\n";
  html += "cursor: not-allowed;\n";
  html += "}\n";

  html += ".inline {\n";
  html += "font-size: 10px !important;\n";
  html += "margin: 0 0 0 10px !important;\n";
  html += "vertical-align: middle ;\n";
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
  html += "<li><a href='/wifiscan'>WiFi Scan</a></li>\n";
  html += "<li><a href='/fwupdate'>FW Update</a></li>\n";
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

void SetConfigVars(DynamicJsonDocument &json)
{
  if (json.containsKey("ssid"))
  {
    cfg.ssid = json["ssid"].as<char *>();
  }

  if (json.containsKey("psk"))
  {
    cfg.psk = json["psk"].as<char *>();
  }

  if (json.containsKey("hostname"))
  {
    if (!json["hostname"].as<String>().isEmpty())
    {
      cfg.hostname = json["hostname"].as<String>();
    }
  }

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

  if (json.containsKey("ntpserver"))
  {
    cfg.ntpserver = json["ntpserver"].as<char *>();
  }

  if (json.containsKey("timeoffset"))
  {
    cfg.timeoffset = json["timeoffset"].as<int>();
  }
}

void SaveConfig()
{
  //save the custom parameters to FS

  DynamicJsonDocument json(1024);
  json["hostname"] = cfg.hostname;
  json["ssid"] = cfg.ssid;
  json["psk"] = cfg.psk;
  json["apiKey"] = cfg.apiKey;
  json["apiUrl"] = cfg.apiUrl;
  json["note"] = cfg.note;
  json["ntpserver"] = cfg.ntpserver;
  json["timeoffset"] = cfg.timeoffset;
  json["password"] = cfg.password;
  json["username"] = cfg.username;
  json["sensorIdxAir"] = cfg.sensorIdxAir;
  json["sensorIdxSolar"] = cfg.sensorIdxSolar;
  json["sensorIdxWater"] = cfg.sensorIdxWater;

  File configFile = LittleFS.open("/config.json", "w");
  serializeJson(json, configFile);
  configFile.close();

  logf("[SaveConfig] Saved\n");

  //end save
}

void LoadConfig()
{
  //file exists, reading and loading
  if (LittleFS.exists("/config.json"))
  {
    File configFile = LittleFS.open("/config.json", "r");

    if (configFile)
    {

      logf("[LoadConfig] Opened config file\n");

      DynamicJsonDocument json(1024);
      auto error = deserializeJson(json, configFile);

      if (error)
      {
        logf("[LoadConfig] deserializeJson() failed with code %s\n", error.c_str());
      }
      else
      {
        SetConfigVars(json);
        logf("[LoadConfig] Loaded\n");
      }
    }
  }
  else
  {

    logf("[LoadConfig] No Configfile, init new file\n");
    SaveConfig();
  }
}

void resetSettings()
{

  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile)
  {

    logf("[resetSettings] Failed to open config file for reset\n");
  }
  configFile.println("");
  configFile.close();
  delay(300);
  ESP.restart();
  delay(300);
}

void setPixel(uint32_t c, uint b = LED_BRIGHTNESS)
{
  static uint32_t lastLEDColor;
  static uint lastLEDBrightness;

  if (lastLEDColor != c || lastLEDBrightness != b)
  {
    pixels.setPixelColor(0, c);
    pixels.setBrightness(b);
    pixels.show();
    lastLEDColor = c;
    lastLEDBrightness = b;
  }
}

void setStatus(uint32_t c, LEDMode mode = LEDMode::Normal)
{
  ledMode = mode;
  ledColor = c;
  ledFirstLoop = true;
  setPixel(c);
}

void setPixelToggle(uint32_t c1, uint32_t c2 = COLOR_BLACK)
{
  static bool last;

  if (ledFirstLoop)
  {
    last = true;
    ledFirstLoop = false;
  }

  if (last)
    setPixel(c1);
  else
    setPixel(c2);

  last = !last;
}

void setPixelFade(uint32_t c1, uint min)
{
  static uint bright;
  static bool direction; // true = down, false = up

  if (ledFirstLoop)
  {
    bright = LED_BRIGHTNESS;
    direction = true;
    ledFirstLoop = false;
  }

  setPixel(ledColor, bright);

  if (direction)
  {
    bright -= 5;
  }
  else
  {
    bright += 5;
  }

  // set direction for next loop
  if (bright >= LED_BRIGHTNESS)
  {
    direction = true; // down
  }
  else if (bright <= min)
  {
    direction = false; // up
  }
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
void printAddress(DeviceAddress deviceAddress, char *buffer, bool upperCase = false)
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
  logf("[initOneWireSensors] Init sensor arrays...\n");

  for (int i = 0; i < MAX_NUM_SENSORS; i++)
  {
    tempSensorValues[i] = DEVICE_DISCONNECTED_C;
    for (int ii = 0; ii < 8; ii++)
    {
      tempSensorAddresses[i][ii] = 0x00;
    }
  }
  logf("[initOneWireSensors] Search Sensors...\n");

  numSensorsFound = sensors.getDeviceCount();
  logf("[initOneWireSensors] Found %d\n", numSensorsFound);

  if (numSensorsFound > 0)
  {

    // select the first sensor
    for (int i = 0; i < numSensorsFound; i++)
    {
      if (sensors.getAddress(tempSensorAddresses[i], i))
      {
        printAddress(tempSensorAddresses[i], buff);
      }
      else
      {
        strcpy(buff, "no address!");
      }
      logf("[initOneWireSensors] %d/%d: %s\n", i + 1, numSensorsFound, buff);
    }
  }
}

void getTemps()
{
  logf("[getTemps] Request temperatures...\n");

  sensors.requestTemperatures();

  for (uint8_t i = 0; i < numSensorsFound; i++)
  {
    tempSensorValues[i] = sensors.getTempC(tempSensorAddresses[i]);

    if (tempSensorValues[i] == DEVICE_DISCONNECTED_C)
    {
      logf("[getTemps] #%i: disconnected\n", i);
    }
    else
    {
      logf("[getTemps] #%i: %.01f °C\n", i, tempSensorValues[i]);
    }
  }
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

  logf("[sendData] Start upload...\n");

  getTemps(); // Send the command to get temperatures
  lcd.clear();
  lcdPrintHeader();
  lcd.setCursor(0, 1);
  lcd.print(F("> Upload data..."));
  lcd.blink();

  // Allocate a temporary JsonDocument
  // Use arduinojson.org/v6/assistant to compute the capacity.
  StaticJsonDocument<500> jsondoc; //

  jsondoc["hostname"] = WiFi.hostname();
  jsondoc["firmware"] = FIRMWARE_VERSION;
  jsondoc["timestamp"] = timeClient.getEpochTime();
  jsondoc["wifi_rssi"] = WiFi.RSSI();
  jsondoc["light"] = analogRead(PIN_LIGHTSENSOR);
  jsondoc["highwater"] = highwater;
  jsondoc["uptime"] = millis();
  jsondoc["mac"] = WiFi.macAddress();

  // Add Temps
  char addressBuffer[16];
  JsonObject temps = jsondoc.createNestedObject("temps");
  for (uint8_t i = 0; i < numSensorsFound; i++)
  {
    if (tempSensorValues[i] != DEVICE_DISCONNECTED_C)
    {
      printAddress(tempSensorAddresses[i], addressBuffer);
      temps[addressBuffer] = tempSensorValues[i];
    }
  }

  yield();

  logf("[sendData] Sending: ");
#ifdef DEBUG
  serializeJson(jsondoc, Serial);
#endif
  logf("\n");

  char JSONmessageBuffer[500];
  serializeJson(jsondoc, JSONmessageBuffer, sizeof(JSONmessageBuffer));

  std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);

  client->setInsecure(); //client->setFingerprint(fingerprint);

  HTTPClient https;

  logf("[sendData] HTTPS Client begin...\n");

  setStatus(COLOR_BLUE);

  if (https.begin(*client, cfg.apiUrl))
  { // HTTPS

    https.addHeader("Content-Type", "application/json"); // Add Header
    https.addHeader("X-API-Key", cfg.apiKey);            // Add Header

    logf("[sendData] HTTP POST data...\n");

    // start connection and send HTTP header
    int httpCode = https.POST(JSONmessageBuffer); // Send the request

    yield();

    // httpCode will be negative on error
    if (httpCode > 0)
    {
      // HTTP header has been send and Server response header has been handled
      logf("[sendData] HTTP Code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        failed = false;
        // logf("[sendData] HTTP Payload: %s", https.getString());
      }
    }
    else
    {
      logf("[sendData] HTTP POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end();
  }
  else
  {
    logf("[sendData] HTTP Unable to connect\n");
  }

  if (failed)
  {
    setStatus(COLOR_RED);
    lcd.noBlink();
    lcd.setCursor(0, 2);
    lcd.print(F("> failed!        "));
    delay(1000);
    screen.reset();
    uploadStatus = UploadStatus::Failed;
  }
  else
  {
    setStatus(COLOR_GREEN, LEDMode::Fade);
    lcd.noBlink();
    lcd.setCursor(0, 2);
    lcd.print(F("> synced!        "));
    delay(1000);
    screen.reset();
    uploadStatus = UploadStatus::Success;
  }

  logf("[sendData] finished\n");

  lastSendData = millis();
  forceSendData = false;
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
  static bool prevBntState;
  bool btnState = digitalRead(PIN_TOUCHSENSOR);

  if (btnState) // pressed
  {
    setPixel(COLOR_TOUCH);
    if (btnState != prevBntState) // was not pressed and is now pressed
    {
      logf("[handleButton] Button pressed short\n");

      buttonTimer = millis();
      screen.nextScreen();

      logf("[handleButton] Current screen: %i\n", screen.currentScreen());
    }

    if ((millis() - buttonTimer >= TIME_BUTTON_LONGPRESS))
    {
      logf("[handleButton] Button pressed long\n");
      sendData();
    }

    // Delay a little bit to avoid bouncing
    delay(50);
  }
  prevBntState = btnState;
}

void wifiConnect()
{

  WiFi.mode(WIFI_OFF);

  // AP or Infrastructire mode
  if (cfg.ssid.isEmpty())
  {
    wifiStatus = WiFiStatus::Hotspot;
    // Start AP

    logf("[wifiConnect] Default Config loaded.\n");
    logf("[wifiConnect] Starting WiFi SoftAP\n");

    WiFi.softAP("LakeTempESP", "");

    lcd.clear();
    lcdPrintHeader();
    lcd.setCursor(0, 1);
    lcd.print(F("> No WiFi config!"));
    lcd.setCursor(0, 2);
    lcd.print(F("> Hotspot Mode..."));
    lcd.setCursor(17, 2);
    lcd.blink();
  }
  else
  {

    logf("[wifiConnect] Connecing to '%s'. Please wait", cfg.ssid.c_str());

    lcd.clear();
    lcdPrintHeader();
    lcd.setCursor(0, 1);
    lcd.print(F("> Connecting to"));
    lcd.setCursor(0, 2);
    lcd.print(F("> WiFi Network..."));
    lcd.blink();

    setStatus(COLOR_BLUE);

    WiFi.mode(WIFI_STA);
    WiFi.hostname(cfg.hostname);
    WiFi.begin(cfg.ssid, cfg.psk);

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(250);
      logf(".");
      setPixelToggle(COLOR_BLUE);
    }
    setStatus(COLOR_BLUE);
    wifiStatus = WiFiStatus::Client;

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

    logf("\n[wifiConnect] WiFi connected!\n");
    logf("> IP: %s\n", WiFi.localIP().toString().c_str());
    logf("> Subnetmask: %s\n", WiFi.subnetMask().toString().c_str());
    logf("> GW: %s\n", WiFi.gatewayIP().toString().c_str());
    logf("> DNS: %s\n", WiFi.dnsIP().toString().c_str());
    logf("> WiFi Diag:\n");
#ifdef DEBUG
    WiFi.printDiag(Serial);
#endif
    logf("\n");
    delay(1000);
    screen.reset();
  }
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

void handleWiFiScan()
{
  showWEBAction();
  if (!server.authenticate(cfg.username.c_str(), cfg.password.c_str()))
  {
    return server.requestAuthentication();
  }
  else
  {

    HTMLHeader("WiFi Scan");

    int n = WiFi.scanNetworks();
    if (n == 0)
    {
      html += "No networks found.\n";
    }
    else
    {
      html += "<table>\n";
      html += "<tr>\n";
      html += "<th>#</th>\n";
      html += "<th>SSID</th>\n";
      html += "<th>Channel</th>\n";
      html += "<th>Signal</th>\n";
      html += "<th>RSSI</th>\n";
      html += "<th>Encryption</th>\n";
      html += "<th>BSSID</th>\n";
      html += "</tr>\n";
      for (int i = 0; i < n; ++i)
      {
        html += "<tr>\n";
        snprintf(buff, sizeof(buff), "%02d", (i + 1));
        html += String("<td>") + buff + String("</td>");
        html += "<td>\n";
        if (WiFi.isHidden(i))
        {
          html += "[hidden SSID]";
        }
        else
        {
          html += "<a href='/settings?ssid=";
          html += WiFi.SSID(i).c_str();
          html += "'>";
          html += WiFi.SSID(i).c_str();
          html += "</a>";
        }
        html += "</td>\n<td>";
        html += WiFi.channel(i);
        html += "</td>\n<td>";
        html += dBm2Quality(WiFi.RSSI(i));
        html += "%</td>\n<td>";
        html += WiFi.RSSI(i);
        html += "dBm</td>\n<td>";
        switch (WiFi.encryptionType(i))
        {
        case ENC_TYPE_WEP: // 5
          html += "WEP";
          break;
        case ENC_TYPE_TKIP: // 2
          html += "WPA TKIP";
          break;
        case ENC_TYPE_CCMP: // 4
          html += "WPA2 CCMP";
          break;
        case ENC_TYPE_NONE: // 7
          html += "OPEN";
          break;
        case ENC_TYPE_AUTO: // 8
          html += "WPA";
          break;
        }
        html += "</td>\n<td>";
        html += WiFi.BSSIDstr(i).c_str();
        html += "</td>\n";
        html += "</tr>\n";
      }
      html += "</table>";
    }

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

      HTMLHeader("Reboot", REFRESH_TIME, "/");
      html += ">>> Reboot in progress... <<<";
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

void printSensorOptionList(uint8_t selectedValue)
{
  char buffer[50];

  for (uint8_t i = 0; i < numSensorsFound; i++)
  {
    printAddress(tempSensorAddresses[i], buffer, true);

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

        if (server.argName(i) == "hostname")
        {
          cfg.hostname = value;
        }
        else if (server.argName(i) == "ssid")
        {
          cfg.ssid = value;
        }
        else if (server.argName(i) == "psk")
        {
          cfg.psk = value;
        }
        else if (server.argName(i) == "apiKey")
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
        else if (server.argName(i) == "ntpserver")
        {
          cfg.ntpserver = value;
        }
        else if (server.argName(i) == "timeoffset")
        {
          cfg.timeoffset = value.toInt();
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
      HTMLHeader("Settings", REFRESH_TIME, "/settings");
      html += ">>> New Settings saved! Reboot in progress... <<<";
    }
    else
    {
      HTMLHeader("Settings");
      html += "<form action='/settings' method='post'>\n";
      html += "<table>\n";

      html += "<tr>\n";
      html += "<td>Hostname:</td>\n";
      html += "<td><input name='hostname' type='text' maxlength='30' autocapitalize='none' placeholder='";
      html += WiFi.hostname().c_str();
      html += "' value='";
      html += cfg.hostname;
      html += "'></td></tr>\n";

      html += "<tr>\n<td>\nSSID:</td>\n";
      html += "<td><input name='ssid' type='text' autocapitalize='none' maxlength='30' value='";
      bool showssidfromcfg = true;
      if (server.method() == HTTP_GET)
      {
        if (server.arg("ssid") != "")
        {
          html += server.arg("ssid");
          showssidfromcfg = false;
        }
      }
      if (showssidfromcfg)
      {
        html += cfg.ssid;
      }
      html += "'><a href='/wifiscan' onclick='return confirm(\"Go to scan site? Changes will be lost!\")'><button type='button' class='inline'>Scan</button></a></td>\n</tr>\n";

      html += "<tr>\n<td>\nPSK:</td>\n";
      html += "<td><input name='psk' type='password' maxlength='30' value='";
      html += cfg.psk;
      html += "'></td>\n</tr>\n";

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

      html += "<tr>\n<td>NTP Server:</td>\n";
      html += "<td><input name='ntpserver' type='text' autocapitalize='none' value='";
      html += cfg.ntpserver;
      html += "'></td></tr>\n";

      html += "<tr>\n<td>NTP Time Offset:</td>\n";
      html += "<td><input name='timeoffset' type='text' autocapitalize='none' value='";
      html += cfg.timeoffset;
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
      SaveConfig();
      delay(500);
      ESP.reset();
    }
  }
}

String millis2Time(unsigned long timestamp)
{
  char timebuff[20];
  int sec = timestamp / 1000;
  int min = sec / 60;
  int hr = min / 60;
  int days = hr / 24;
  snprintf(timebuff, 20, " %02d:%02d:%02d:%02d", days, hr % 24, min % 60, sec % 60);

  return timebuff;
}

void handleRoot()
{

  logf("[handleRoot] Webserver access\n");

  showWEBAction();

  String value;
  if (server.method() == HTTP_POST)
  {
    lastSendData = millis();
    forceSendData = true;
    //sendData();
  }

  HTMLHeader("Main");

  html += "<form action='/' method='post'>\n";
  html += "<table>\n";

  html += "<tr>\n<td>Uptime:</td>\n<td>";
  html += millis2Time(millis());
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

  html += "<tr>\n<td>Last upload:</td>\n<td>";
  html += millis2Time(millis() - lastSendData);
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>Next upload:</td>\n<td>";
  html += millis2Time((lastSendData + SENDDATA_INTERVAL) - millis());
  html += "<input type='submit' value='Upload now' class='inline'>\n";
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>OneWire Sensors:</td>\n<td>";
  for (uint8_t i = 0; i < numSensorsFound; i++)
  {
    snprintf(buff, 50, "%02d/%02d: %.1f", i + 1, numSensorsFound, tempSensorValues[i]);
    html += buff;
    html += "&deg;C (<code>";
    printAddress(tempSensorAddresses[i], buff, true);
    html += buff;
    html += "</code>)<br />";
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
  if (cfg.note.isEmpty())
  {
    html += "---";
  }
  else
  {
    html += cfg.note;
  }
  html += "</td>\n</tr>\n";

  html += "<tr>\n<td>Hostname:</td>\n<td>";
  html += cfg.hostname;
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

  html += "<tr>\n<td>Free Sketch Space</td>\n<td>";
  html += ESP.getFreeSketchSpace();
  html += " bytes</td>\n</tr>\n";

  html += "<tr>\n<td>Reset Reason</td>\n<td>";
  html += ESP.getResetReason();
  html += "</td>\n</tr>\n";

  html += "</table>\n";
  html += "</form>\n";

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

void handleResetOnBoot()
{
  unsigned long timer = 0;
  unsigned long timer2 = 0;
  uint8_t countdown = RESET_COUNTDOWN;

  while (digitalRead(PIN_TOUCHSENSOR))
  {
    // LED
    if (timer2 == 0 || millis() - timer2 > ((countdown * 1000) / 25))
    {
      setPixelToggle(COLOR_RED);
      timer2 = millis();
    }

    // LCD
    if (timer == 0 || millis() - timer > 1000)
    {
      if (countdown == 0)
      {
        setStatus(COLOR_RED);
        lcd.clear();
        lcdPrintHeader();
        lcd.setCursor(0, 1);
        lcd.print(F("> Factory defaults"));
        lcd.setCursor(0, 2);
        lcd.print(F("> restored!"));
        lcd.setCursor(0, 3);
        lcd.print(F("> Rebooting... "));
        resetSettings();
      }
      else
      {
        if (countdown == RESET_COUNTDOWN)
        {
          lcd.clear();
          lcdPrintHeader();
          lcd.setCursor(0, 1);
          lcd.print(F("> Factory reset "));
          lcd.setCursor(0, 2);
          lcd.print(F("> in    seconds..."));
        }

        lcd.setCursor(5, 2);
        lcd.printf("%02d ", countdown);

        timer = millis();
        countdown = countdown - 1;
      }
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
}

void setup(void)
{
#ifdef DEBUG
  Serial.begin(115200);
  delay(100);
#endif
  logf("\n\n == Welcome to LakeTemp v%s ==\n", FIRMWARE_VERSION);

  // Mounting FileSystem
  logf("[setup] Mounting file system...\n");

  if (LittleFS.begin())
  {
    logf("[setup] Mounted file system.\n");
    LoadConfig();
  }
  else
  {
    logf("[setup] Failed to mount FS\n");
  }

  // Setting the I/O pin modes
  pinMode(PIN_TOUCHSENSOR, INPUT);
  pinMode(PIN_WATERSENSOR, INPUT);

  // NeoPixel
  pixels.begin();
  pixels.clear();
  setStatus(COLOR_YELLOW);

  // BME Begin
  logf("[setup] Searching BME280 sensor...\n");
  if (!bme.begin(BME280_ADDRESS_ALTERNATE))
  {
    logf("Could not find a valid BME280 sensor, check wiring, address, sensor ID!\n");
  }
  else
  {
    logf("[setup] Found valid BME280 sensor!\n");
    logf("> Temperature = %f °C\n", bme.readTemperature());
    logf("> Pressure = %f hPa\n", bme.readPressure() / 100.0F);
    logf("> Humidity = %f %%\n", bme.readHumidity());
  }

  // OneWire
  logf("[setup] Searching OneWire sensors...\n");

  sensors.begin();
  initOneWireSensors();
  getTemps();

  // LCD begin
  while (lcd.begin(LCD_COLUMS, LCD_ROWS) != 1)
  {

    logf("[setup] PCF8574 is not connected\n");

    delay(5000);
  }

  logf("[setup] PCF8574 connected!\n");

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

  handleResetOnBoot();

  // lcd.backlight();
  // delay(500);
  // lcd.noBacklight();
  // delay(500);
  // lcd.backlight();
  // delay(500);
  // lcd.noBacklight();
  // delay(500);
  // lcd.backlight();
  // delay(500);
  // lcd.noBacklight();
  // delay(500);
  // lcd.backlight();
  // delay(500);
  // lcd.noBacklight();
  // delay(500);
  // lcd.backlight();

  // Wifi connect
  wifiConnect();

  // NTPCLient
  if (!cfg.ntpserver.isEmpty())
  {
    timeClient.setPoolServerName(cfg.ntpserver.c_str());
    if (cfg.timeoffset > 0)
    {
      timeClient.setTimeOffset(cfg.timeoffset);
    }
    timeClient.setUpdateInterval(60 * 60 * 1000); // in ms
    timeClient.begin();
  }

  // Arduino OTA Update
  httpUpdater.setup(&server, "/dofwupdate", cfg.username, cfg.password);

  // Webserver
  server.on("/", handleRoot);
  server.on("/reboot", handleReboot);
  server.on("/settings", handleSettings);
  server.on("/wifiscan", handleWiFiScan);
  server.on("/fwupdate", handleFWUpdate);
  server.onNotFound(handleNotFound);
  server.begin();

  if (wifiStatus == WiFiStatus::Client)
  {
    setStatus(COLOR_GREEN, LEDMode::Fade);
  }
  else if (wifiStatus == WiFiStatus::Hotspot)
  {
    setStatus(COLOR_BLUE, LEDMode::Blink);
  }
}

void loop()
{

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

  if (wifiStatus == WiFiStatus::Client)
  {
    // NTPCLient
    lastTimeSyncWasSuccessfull = timeClient.update();

    // handleButton
    handleButton();

    // Send Data block
    if (millis() - lastSendData >= SENDDATA_INTERVAL || forceSendData)
    {
      sendData();
    }

    screen.loop();

    // LCD
    lcdShow();
  }

  // LED Modes
  if (ledMode == LEDMode::Blink)
  {
    if (millis() - ledTimer > LED_BLINK_INTERVAL)
    {
      setPixelToggle(ledColor);
      ledTimer = millis();
    }
  }
  else if (ledMode == LEDMode::Fade)
  {
    if (millis() - ledTimer > LED_FADE_INTERVAL)
    {
      setPixelFade(ledColor, 20);
      ledTimer = millis();
    }
  }
  else
  {
    setPixel(ledColor);
  }
}