/*
  Project: Ring Turn Page
  Author: Duncan Mwanik
*/
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "OneButton.h"
#include "BleKeyboard.h"

OneButton button1(5, true); //initialize button press identifier
BleKeyboard bleKeyboard;

// BLE constants
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

const int pinBatteryVoltage = 12;
const int pinBatteryCharging = 18;
const int pinBatteryFull = 13;
const int pinLowBatteryLED = 5;
const int pinBatteryFullLED = 6;
const int pinActionLED = 6;  

const float lowBatteryThreshold = 695; // low battery (3.4V) equivallent


void setup() {

    pinMode(pinBatteryCharging, INPUT);
    pinMode(pinBatteryVoltage, INPUT);
    pinMode(pinBatteryFull, INPUT);
    pinMode(pinLowBatteryLED, OUTPUT);
    pinMode(pinActionLED, OUTPUT);
    pinMode(pinBatteryFullLED, OUTPUT);

    //attach listening for different button presses
    button1.attachClick(click1);
    button1.attachDoubleClick(doubleclick1);
    button1.attachDuringLongPress(longPress1);

    bleKeyboard.begin();
  
}

void loop() {

  // if not charging, continue:
  if (digitalRead(pinBatteryCharging) == LOW) {
    
    //if battery voltage is still high, do main task
    if(analogRead(pinBatteryVoltage) > lowBatteryThreshold) {
        // main code here
        
        BLEDevice::init(""); // Setting the BT name of the ring
        BLEServer *pServer = BLEDevice::createServer();
        BLEService *pService = pServer->createService(SERVICE_UUID);
        BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                                CHARACTERISTIC_UUID,
                                                BLECharacteristic::PROPERTY_READ |
                                                BLECharacteristic::PROPERTY_WRITE
                                            );
        
        pCharacteristic->setValue("Ring Turn Page");
        pService->start();
        // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
        pAdvertising->setMinPreferred(0x12);
        BLEDevice::startAdvertising();


        //initiate listening and processing button press
        button1.tick();
        delay(10);

      
    }
    
    //if battery voltage is low
    else{
      digitalWrite(pinLowBatteryLED, HIGH); //turn on red LED to show low battery status
    }
    
  }
    

  else { //if charger is connected:
    
    //if charging is completed
    if(digitalRead(pinBatteryFull) == HIGH) {
      digitalWrite(pinBatteryFullLED, HIGH); // turn on green LED to show charging complete
    }
    
    //if still charging
    else{
      digitalWrite(pinActionLED, HIGH); // turn on blue led to show charging in progress
    }
    
  }

}


//processing for single button press
void click1() {
    for (int i=1; i<=5; i=i+1) {
            
    digitalWrite(pinActionLED, HIGH); 

    if(bleKeyboard.isConnected()) {

        //navigate to next page/presses right arrow
        bleKeyboard.write(KEY_RIGHT_ARROW);
        delay(100);
    }

    digitalWrite(pinActionLED, LOW); 
    }
} // click1


//processing for double button press
void doubleclick1() {
    for (int i=1; i<=15; i=i+1) { 
                
    digitalWrite(pinActionLED, HIGH); 
    delay(100);
    digitalWrite(pinActionLED, LOW); 
    delay(100);
    digitalWrite(pinActionLED, HIGH); 

    if(bleKeyboard.isConnected()) {

        //navigate to previous page/presses left arrow
        bleKeyboard.write(KEY_LEFT_ARROW);
        delay(100);
    }
    
    digitalWrite(pinActionLED, LOW); 
    }

} // doubleclick1

//processing for long press
void longPress1() {
    digitalWrite(pinActionLED, HIGH); 
    delay(3000);
    digitalWrite(pinActionLED, LOW);

    // make esp32 go into deep sleep to conserve power
    //set source for waking up the esp32: if button is pressed once while in deep sleep
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_5,0);

    //put the esp32 into deep sleep to save power: BT gets disabled as well as other minor tasks
    esp_deep_sleep_start();
} // longPress1
