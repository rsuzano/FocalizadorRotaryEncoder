/////////////////////////////////////////////////////////////////

#include "Button2.h"; //  https://github.com/LennartHennigs/Button2
#include <RotaryEncoder.h>
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
// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder *encoder = nullptr;
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
/////////////////////////////////////////////////////////////////
int posicao;
int microsteps=1;
unsigned long now = 0;
unsigned long lastTrigger = 0;
void setup() {
encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);
   // Declare pins as output:
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  posicao = 0;
  stepper.setEnablePin();  
  stepper.setMaxSpeed(1000);
  stepper.setCurrentPosition(0);
  stepper.setAcceleration(500);
  stepper.setSpeed(900);
  Serial.begin(9600);
   
  Serial.println("\n\nSimple Counter");
  
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
  //(lastTrigger > 0 && (now - lastTrigger) > 50)
  if (pos != newPos) {
    int newDir = (int)(encoder->getDirection());
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println(newDir);
    pos = newPos;
    stepper.moveTo(newPos * microsteps);
    doStep();
  }else{ 
    doStep();
  }
  b.loop();
}

/////////////////////////////////////////////////////////////////
void doStep(){
  digitalWrite(enablePin, LOW);
   stepper.run();
   now = millis();
   if(!stepper.run()){
     digitalWrite(enablePin, HIGH);
     lastTrigger = 0;
   }
    
}
// on change
IRAM_ATTR void rotate() {
    encoder->tick(); // just call tick() to check the state.   
    lastTrigger = millis();
    
}

 
// single click
void click(Button2& btn) {
  if(microsteps==1){
    microsteps=8;
  }else if(microsteps==8){
    microsteps=16;
  }else if(microsteps==16){
    microsteps=32;
  }else if(microsteps==32){
    microsteps=1;
  }else {
     microsteps=1;
  }
  
  Serial.print("MicroSteps:");
  Serial.println(microsteps);
}

// long click
void resetPosition(Button2& btn) {
  posicao = 0;
  stepper.setCurrentPosition(posicao);
  Serial.println("Rotary and Stepper reseted to 0");
  digitalWrite(enablePin, LOW);
}

/////////////////////////////////////////////////////////////////
