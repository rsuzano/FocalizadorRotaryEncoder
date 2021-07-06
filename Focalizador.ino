/*
 * Title       FocalizadorRotaryEncoder
 * by          Regis Costa
 *
 * Copyright (C) 2021 Regis Costa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Description:
 *   
 * Author: Regis Costa
 *   regis@rsuzano.com
 *
 */
/////////////////////////////////////////////////////////////////

#include "Button2.h"; //  https://github.com/LennartHennigs/Button2
#include <RotaryEncoder.h> //https://github.com/mathertel/RotaryEncoder
#include <AccelStepper.h>

/////////////////////////////////////////////////////////////////

#define dirPin D2
#define stepPin D4
#define enablePin D1
#define motorInterfaceType 1



#define PIN_IN1  D5
#define PIN_IN2 D6
#define BUTTON_PIN  D3

/////////////////////////////////////////////////////////////////

Button2 b;

RotaryEncoder *encoder = nullptr;
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
/////////////////////////////////////////////////////////////////
int position;
int precision=1;
unsigned long now = 0;
unsigned long lastTrigger = 0;
void setup() {
encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
   // Declare pins as output:
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  position = 0;
  stepper.setEnablePin();  
  stepper.setMaxSpeed(1000);
  stepper.setCurrentPosition(0);
  stepper.setAcceleration(500);
  stepper.setSpeed(900);
  Serial.begin(9600);
   
  
  b.begin(BUTTON_PIN);
  b.setTapHandler(click);
  b.setLongClickTime(2000);
  b.setLongClickHandler(resetPosition);


  // register interrupt routine
  attachInterrupt(digitalPinToInterrupt(PIN_IN1), rotate, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_IN2), rotate, CHANGE);
}

void loop() {  
  static int pos = 0;
  encoder->tick();
  int newPos = encoder->getPosition();
  
  now = millis();
  
  if (pos != newPos) {
    int newDir = (int)(encoder->getDirection());
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println(newDir);
    pos = newPos;
    stepper.moveTo(newPos * precision);
    doStep();
  }else{ 
    doStep();
  }
  b.loop();
}


void doStep(){
  digitalWrite(enablePin, LOW);
   stepper.run();
   now = millis();
   if(!stepper.run()){
     digitalWrite(enablePin, HIGH);
     lastTrigger = 0;
   }
    
}

IRAM_ATTR void rotate() {
    encoder->tick(); 
    lastTrigger = millis();
    
}

 
void click(Button2& btn) {
  if(precision==1){
    precision=8;
  }else if(precision==8){
    precision=16;
  }else if(precision==16){
    precision=32;
  }else if(precision==32){
    precision=1;
  }else {
     precision=1;
  }
  resetPosition(btn);
  Serial.print("precision:");
  Serial.println(precision);
}

// long click
void resetPosition(Button2& btn) {
  position = 0;
  encoder->setPosition(position);
  stepper.setCurrentPosition(position);
  Serial.println("Rotary and Stepper reseted to 0");
  digitalWrite(enablePin, LOW);
}


