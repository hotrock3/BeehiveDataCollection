#include "Wire.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "HX711_ADC.h"
#include "EEPROM.h"
#include "WiFi.h"
#include "Arduino.h"
#include "ArduinoJson.h"
#include "ArduinoJson.hpp"
#include "PubSubClient.h"

// User Adjustable Settings from here until line
//Sleep Timer : How long will the ESP 32 go to sleep before the next reading?
int sleepTimeInSeconds = 1800;

// Set hive sensor number
int hiveNumber = 1;

// Setup LED indicator
int LED_BUILTIN = 2;

//Wifi Details 
const char* ssid     = "WifiNetworkName";     // Type your network SSID (name)
const char* password = "WifiPassword";        // Type your network password
# define WiFiTimeOutMS 60000                  // How long will it attempt to connect. Value typed = seconds * 1000

//MQTT Detailes  MUST SET IP ADDRESS FOR MQTT
const char* mqtt_server = "192.168.1.100";    //update to be Pi server's IP address. Keystone Guest = 10.254.14.11
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;

// End of user settings - Do not adjust unless you know what you are doing.

// DS18B20 connected to pin 13
#define ONE_WIRE_BUS 13
float CorrectedValue;

// Temp Probe Calibrated Settings
float RawValue =0;
float RawHigh = 100;
float RawLow = 1.1;
float ReferenceHigh = 100;
float ReferenceLow = 0;
float RawRange = RawHigh - RawLow;
float ReferenceRange = ReferenceHigh - ReferenceLow;
int Temp;

// Passing oneWire reference to Dallas Temperature
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensor1(&ourWire);

// Setting HX711 pins:
const int HX711_dout = 32; //mcu > HX711 dout pin
const int HX711_sck = 33; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);
const int calVal_eepromAdress = 0;
const int tareOffsetVal_eepromAdress = 4;
unsigned long t = 0;
int Weight;


void setup(){

  delay(1000);
 
  // LED Setup
  pinMode (LED_BUILTIN, OUTPUT);


  // Start Serial communication at 115200 baud
  Serial.begin(115200);
  //Start Temp sensor
  sensor1.begin();
  sensor1.setResolution(11);
  delay(100);

  // Setup Load Cells
  LoadCell.begin();

  float calibrationValue;
  EEPROM.begin(512); // begin process of getting calibration and tare values
  EEPROM.get(calVal_eepromAdress, calibrationValue); // Getting calibration value from eeprom
  long tare_offset = 0;
  EEPROM.get(tareOffsetVal_eepromAdress, tare_offset); // Getting tare offset from eeprom
  LoadCell.setTareOffset(tare_offset);

  unsigned long stabilizingtime = 0; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = false; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Load Cell Startup is complete");
  }

  // Set MQTT Server Details
  client.setServer(mqtt_server, 1883);

}

void getTemp(){
  sensor1.requestTemperatures();
  // Adjusting Calibration
  float RawValue = sensor1.getTempCByIndex(0);
  CorrectedValue = (((RawValue - RawLow) * ReferenceRange) / RawRange) + ReferenceLow;
  Serial.print("Sens. 1 ");
  Serial.println(RawValue, 1);
  Serial.print("Corrected. 1 ");
  Serial.println(CorrectedValue, 1);
  Temp = CorrectedValue * 10;
  Serial.println(Temp);
  delay(1000);
}

void getWeight(){
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  for (int i=0; i<30; i++) {
    // You can increase or decreas the count based on how quickly your load cell readings settle
    //check for new data/start next conversion:
    if (LoadCell.update()) newDataReady = true;

    // get smoothed value from the dataset:
    if (newDataReady) {
      if (millis() > t + serialPrintInterval) {
        float w = LoadCell.getData();
        Serial.print("Load_cell output val: ");
        Serial.println(w);
        newDataReady = 0;
        t = millis();
        delay(100);
        Weight = w * 100;
      }
    }
  }
}

void connectToWiFi(){
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    unsigned long startAttemptTime = millis();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password); 
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WiFiTimeOutMS)
    {
      Serial.print(".");
      delay(200);
    }
  }

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("\nFailed!");
    // take action
  }else{
    Serial.print("\nConnected to ");
    Serial.print(ssid);
    Serial.print(" at ");
    Serial.println(WiFi.localIP());
  }
}

void connectMQTT() {
  if(!client.connected()){
    Serial.println("MQTT not connected... Trying to connect");
    unsigned long startAttemptTime = millis();

    // Create Client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);


    // Attempt connection with possible time out
    while (!client.connected() && millis() - startAttemptTime < WiFiTimeOutMS) {
      // Attempt connection
      if(client.connect(clientId.c_str())){
       Serial.println("Connected Successfully");
      }
      else{
        Serial.print("Failed, code= ");
        Serial.println(client.state());
        delay(500);
      }
    }
  }
}

void loop(){
  // Print empty line and wait to make sure Serial is ready.
  delay(200);

  // LED single blink to confirm working
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  
  Serial.println("");

  // Getting Temperature
  getTemp();
  delay(50);

  // Getting Weight
  getWeight();
  delay(50);

  // Connecting to WiFi
  connectToWiFi();
  delay(50);

  // Check if connected to MQTT Server
  if(!client.connected()){
    connectMQTT();
  }
  delay(50);

  StaticJsonDocument<80> doc;
  char output[80];

    // Testing values comment if not testing 
    // Temp = 91 * 10;  //Tempvalue * 10
    // Weight = 65;

    //Add variables to JSON document
    doc["t"] = Temp;
    doc["w"] = Weight;
    doc["h"] = hiveNumber;

    // Serialize JSON and send
    serializeJson(doc, output);
    Serial.println(output);
    client.publish("/hive/sensors", output);
    delay(10000);
  
  WiFi.disconnect();

  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);

  // Set the sleep timer and then enter deep sleep
  esp_sleep_enable_timer_wakeup(sleepTimeInSeconds * 1000000); // The math converts seconds into microseconds
  delay(50);
  esp_deep_sleep_start();

}