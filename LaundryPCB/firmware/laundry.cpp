// ******* WASHING MACHINE CONTROLLER using WIFI
// Notifying control software about material left in the box not yet implemented
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASS";

const int START = 4;
const int STOP = 5;

// we use the solenoid taps rate of flow for each material and time
// to know what amount to dispense into the washing machine
const int WATER = 13;
const int HOT_WATER = 12;
const int BLEACH = 16;
const int POWDER = 14;
const int SOFTENER = 15;
const int BUZZER = 18;
const int WASHER = 18;
const int DRYER = 18;

// we use 
const int SENSOR_WATER = 2;
const int SENSOR_POWDER = 2;
const int SENSOR_BLEACH = 2;
const int SENSOR_SOFTENER = 2;

bool available = true;
bool start = false;

const char* get_order = "http://IP_ADDRESS_CONTROL_SOFTWARE/order";
const char* get_water = "http://IP_ADDRESS_CONTROL_SOFTWARE/water";
const char* get_hot_water = "http://IP_ADDRESS_CONTROL_SOFTWARE/hotwater";
const char* get_powder = "http://IP_ADDRESS_CONTROL_SOFTWARE/powder";
const char* get_bleach = "http://IP_ADDRESS_CONTROL_SOFTWARE/bleach";
const char* get_softener_size = "http://IP_ADDRESS_CONTROL_SOFTWARE/softener";
const char* get_start = "http://IP_ADDRESS_CONTROL_SOFTWARE/start";

String order = "none";
String hot_water_size;
String water_size;
String powder_size;
String bleach_size;
String softener_size;
String hot_water_size;
String start_str;

void setup() {

  pinMode(START, INPUT_PULLUP);
  pinMode(STOP, INPUT_PULLUP);
  pinMode(WATER, OUTPUT);
  pinMode(HOT_WATER, OUTPUT);
  pinMode(POWDER, OUTPUT);
  pinMode(BLEACH, OUTPUT);
  pinMode(SOFTENER, OUTPUT);
  pinMode(SENSOR_WATER, INPUT);
  pinMode(SENSOR_POWDER, INPUT);
  pinMode(SENSOR_BLEACH, INPUT);
  pinMode(SENSOR_SOFTENER, INPUT);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) { // turn LED on to show board is connected to WIFI
    delay(500);
    digitalWrite(GPIO_NUM_2, HIGH);
  }
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){
    // if there is no order in progress
    if(order == "none"){
      order = httpGETRequest(get_order);
      hot_water_size = httpGETRequest(get_hot_water);
      water_size = httpGETRequest(get_water);
      powder_size = httpGETRequest(get_powder);
      bleach_size = httpGETRequest(get_bleach);
      softener_size = httpGETRequest(get_softener_size);
      
      // convert material values to int since these are time delay values
      int hot_water_value = hot_water_size.toInt();
      int water_value = water_size.toInt();
      int powder_value = powder_size.toInt();
      int bleach_value = bleach_size.toInt();
      int softener_value= softener_size.toInt();

      // if order is not empty, release material to the machine
      if(order != ""){
        // if the material value is 0, then that material won't be released
        // turn on each solenoid tap for the specified time period
        if(hot_water_value != 0){
          digitalWrite(HOT_WATER, HIGH);
          delay(hot_water_value);
          digitalWrite(HOT_WATER, LOW);
        }
        if(water_value != 0){
          digitalWrite(WATER, HIGH);
          delay(water_value);
          digitalWrite(WATER, LOW);
        }
        if(powder_value != 0){
          digitalWrite(POWDER, HIGH);
          delay(powder_value);
          digitalWrite(POWDER, LOW);
        }
        if(bleach_value != 0){
          digitalWrite(BLEACH, HIGH);
          delay(bleach_value);
          digitalWrite(BLEACH, LOW);
        }
        if(softener_value != 0){
          digitalWrite(SOFTENER, HIGH);
          delay(softener_value);
          digitalWrite(SOFTENER, LOW);
        }

      }
    }
    // if there is an order already in progress
    else{
      // get status value from control software
      start_str = httpGETRequest(get_start);
      // if START control button has been pressed
      if(digitalRead(START)){
        start_str = "1";
      }
      // if STOP control button has been pressed
      if(digitalRead(STOP)){
        start_str = "0";
      }
      // if 1, start washing process
      if(start_str=="1"){

        // DO WASHING PROCESS CODE

      }
      // if 0, stop washing process
      else if(start_str == "0"){
        
        // STOP WASHING PROCESS CODE

      }
      // if 2, cancel the order
      else if(start_str == "2"){

        order = "none";

      }

    }

  }
  else{ // flash LED on and off to show board has not connected to WIFI
    digitalWrite(GPIO_NUM_2, HIGH);
    delay(300);
    digitalWrite(GPIO_NUM_2, HIGH);
  }

}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}