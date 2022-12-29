/*
  Project: Smart Tap
  Author: Duncan Mwanik
  Date Rev: 27/02/2022
*/
#include <Servo.h>
Servo servo;

const int pinBottomIR = A0;
const int pinSideIR = A1;
const int pinBatteryVoltage = A3;
const int pinLowBatteryLED = A4;
const float lowBatteryThreshold = 695; // low battery (3.4V) equivallent
const int valueIRThreshold = 100;

void setup() {
  Serial.begin(9600);

  servo.attach(9);
  pinMode(pinBottomIR, INPUT);
  pinMode(pinSideIR, INPUT);
  pinMode(pinBatteryVoltage, INPUT);
  pinMode(pinLowBatteryLED, OUTPUT);

}

void loop() {

  // if the battery voltage is high enough, continue, ELSE turn on low battery level LED:
  int batteryVoltage = analogRead(pinBatteryVoltage);

  // if the battery voltage is high enough, continue:
  if (batteryVoltage > lowBatteryThreshold) {
    digitalWrite(pinLowBatteryLED, LOW);

    int valueSideIR1 = analogRead(pinSideIR);
    int valueBottomIR1 = analogRead(pinBottomIR);

    // if the bottom sensor detects hands:
    if (valueBottomIR1 < valueIRThreshold) {
      // turn servo from 0 to 90 degrees to open tap
      servo.write(90);

      // if hand is still detected, servo continues to open the tap:
      while (analogRead(pinBottomIR) < valueIRThreshold) {
        delay(0);
      }

      // if hand is withdrawn, servo closes the tap by turning to original position
      servo.write(0);

    }

    // if the side sensor detects object:
    else if (valueSideIR1 < valueIRThreshold) {
      // turn servo from 0 to 10 degrees to open tap:
      servo.write(10);
      delay(3000); // no object detection for 3s : avoid false tap closing

      // wait for 3 minutes:
      for (int i = 0; i <= 8000; i++) {
        delay(1);

        //if object is detected again by side sensor stop time wait:
        if (analogRead(pinSideIR) < valueIRThreshold) {
          break;
        }
      }

      // servo closes the tap by turning to original position after
      // time wait ends or is interrupted
      servo.write(0);
      delay(3000); // no object detection for 3s : avoid false tap opening

    }

  } else {
    digitalWrite(pinLowBatteryLED, HIGH); // turn on low battery LED indication
  }

}

