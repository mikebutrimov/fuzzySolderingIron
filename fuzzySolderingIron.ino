#include <FuzzyRuleAntecedent.h>
#include <FuzzySet.h>
#include <FuzzyRuleConsequent.h>
#include <Fuzzy.h>
#include <FuzzyRule.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
 
int oldTemperature = 220;
int newTemperature = 220;
int defaultTemperature = 220;
int maxTemperature = 300;
int pwnOutputPin = 5;
float pwmThrottle = 0;
int temperatureInputPin = 18;
int currentTime = 0; 
int lastTimeCall = 0;
int secondToLastTimeCall;
int upButtonPin = 19;
int downButtonPin = 20;
unsigned long coolDownOnce = 0; 
unsigned long coolDownRepeat = 0; 
unsigned long coolDownOnStartRepeat = 0; 
int coolDownOnceTrigger = 0;
int coolDownRepeatTrigger = 0;
int coolDownOnStartRepeatTrigger = 0;
int upButtonState = 0;
int upButtonIsPressed = 0;
int upButtonWasPressed= 0;
 
int downButtonState = 0;
int downButtonIsPressed = 0;
int downButtonWasPressed = 0;

int contDelay = 100;
int onceDelay = 20;
int onStartRepeatDelay = 500;

Fuzzy* fuzzy;
FuzzyInput* temperature;
FuzzySet* lowTemperature;
FuzzySet* normalTemperature;
FuzzySet* highTemperature;
FuzzyOutput* action;
FuzzySet* up;
FuzzySet* down;
FuzzySet* normal;
FuzzyRuleAntecedent *ifTemperatureLow, *ifTemperatureNormal, *ifTemperatureHigh;
FuzzyRuleConsequent *thenActionUp, *thenActionDown, *thenActionNormal;
FuzzyRule *fuzzyRule01, *fuzzyRule02, *fuzzyRule03;
 
void setUpFuzzy (int temp){
  fuzzy = new Fuzzy();
  temperature = new FuzzyInput(1);
  action = new FuzzyOutput(1);
  lowTemperature = new FuzzySet(0,0,temp-5,temp-1);
  normalTemperature = new FuzzySet(temp-1,temp,temp,temp+1);
  highTemperature = new FuzzySet(temp+1,temp+10,temp+20,temp+20);
  temperature->addFuzzySet(lowTemperature);
  temperature->addFuzzySet(normalTemperature);
  temperature->addFuzzySet(highTemperature);
  fuzzy->addFuzzyInput(temperature);
  up = new FuzzySet(100, 110, 220,250);
  down = new FuzzySet(0, 0, 0, 0);
  normal = new FuzzySet(30, 40, 90, 100);
  action->addFuzzySet(up);
  action->addFuzzySet(down);
  action->addFuzzySet(normal);
  fuzzy->addFuzzyOutput(action);  
  //rules
  //if temperature  - > low, action - > up
  ifTemperatureLow = new FuzzyRuleAntecedent();
  ifTemperatureLow->joinSingle(lowTemperature);
  thenActionUp = new FuzzyRuleConsequent();
  thenActionUp->addOutput(up);
  fuzzyRule01 = new FuzzyRule(1, ifTemperatureLow, thenActionUp);
  fuzzy->addFuzzyRule(fuzzyRule01);
  //normal
  ifTemperatureNormal = new FuzzyRuleAntecedent();
  ifTemperatureNormal->joinSingle(normalTemperature);
  thenActionNormal = new FuzzyRuleConsequent();
  thenActionNormal->addOutput(normal);
  fuzzyRule02 = new FuzzyRule(1, ifTemperatureNormal, thenActionNormal);
  fuzzy->addFuzzyRule(fuzzyRule02);    
  //if temperature - > high, action - down
  ifTemperatureHigh = new FuzzyRuleAntecedent();
  ifTemperatureHigh->joinSingle(highTemperature);
  thenActionDown = new FuzzyRuleConsequent();
  thenActionDown->addOutput(down);
  fuzzyRule03 = new FuzzyRule(1, ifTemperatureHigh, thenActionDown);
  fuzzy->addFuzzyRule(fuzzyRule03);
}
 
void deleteFuzzy(){
  delete temperature;
  delete lowTemperature;
  delete normalTemperature;
  delete highTemperature;
  delete up;
  delete down;
  delete normal;
  delete ifTemperatureLow;
  delete ifTemperatureNormal;
  delete ifTemperatureHigh;
  delete thenActionUp;
  delete thenActionDown;
  delete thenActionNormal;
  delete fuzzyRule01;
  delete fuzzyRule02;
  delete fuzzyRule03;
  delete fuzzy;
  delete action;
}
 
void processButtons(){
  upButtonState = digitalRead(upButtonPin);
  downButtonState = digitalRead(downButtonPin);
  if (upButtonState == 0 && upButtonIsPressed == 0){
    //do fucking nothing
  }
  if (downButtonState == 0 && downButtonIsPressed == 0){
    //do fucking nothing
  }
  if (upButtonState == 1 && upButtonIsPressed == 0){
    //rise fucking temperature
    newTemperature = riseTemperature(oldTemperature);
    upButtonIsPressed = 1;
  }
  if (downButtonState == 1 && downButtonIsPressed == 0){
    //lower fucking temperature
    newTemperature = lowerTemperature(oldTemperature);
    downButtonIsPressed = 1;
  }
  if (upButtonState == 1 && upButtonIsPressed == 1){
    if (coolDownOnStartRepeatTrigger == 0){
      coolDownOnStartRepeat = millis() + onStartRepeatDelay;
      coolDownOnStartRepeatTrigger = 1;
    }
    if (millis() > coolDownOnStartRepeat){      
      //rise fucking temperature
      if (coolDownRepeatTrigger == 0){
      coolDownRepeat = millis()+contDelay;
      coolDownRepeatTrigger = 1;
    }
    if (millis() > coolDownRepeat){  
      newTemperature = riseTemperature(oldTemperature);
      coolDownRepeatTrigger = 0;
    }
    }
  upButtonIsPressed = 1;
  }
  if (downButtonState == 1 && downButtonIsPressed == 1){
    //lower fucking temperature
      if (coolDownOnStartRepeatTrigger == 0){
        coolDownOnStartRepeat = millis() + onStartRepeatDelay;
        coolDownOnStartRepeatTrigger = 1;
      }
    if (millis() > coolDownOnStartRepeat){      
      if (coolDownRepeatTrigger == 0){
      coolDownRepeat = millis()+contDelay;
      coolDownRepeatTrigger = 1;
      }
      if (millis() > coolDownRepeat){  
        newTemperature = lowerTemperature(oldTemperature);
        coolDownRepeatTrigger = 0;
      }
    }
    downButtonIsPressed = 1;
  }
  if (upButtonState == 0 && upButtonIsPressed == 1){
    //release fucking button
    upButtonIsPressed = 0;
    coolDownRepeatTrigger = 0;
    coolDownOnStartRepeatTrigger = 0;
  }
  if (downButtonState == 0 && downButtonIsPressed == 1){
    //release fucking button
    downButtonIsPressed = 0;
    coolDownRepeatTrigger = 0;
    coolDownOnStartRepeatTrigger = 0;
  }
}
 
int riseTemperature (int temp){
  int returnedTemp = temp+5;
  if (returnedTemp <= maxTemperature){
    return returnedTemp;
  }
  else {
    return temp;
  }
}
 
int lowerTemperature (int temp){
  int returnedTemp = temp-5;
  if (returnedTemp < 0){
    return temp;
  }
  return returnedTemp;
}
 
 
 
void setup() {
  Serial.begin(115200);
  pinMode(pwnOutputPin, OUTPUT);
  pinMode(temperatureInputPin, INPUT);
  pinMode(upButtonPin, INPUT);
  pinMode(downButtonPin, INPUT);
  analogWrite(pwnOutputPin,pwmThrottle);
  setUpFuzzy(defaultTemperature);
}
 
 
 
void loop() {
  //here we read out temperature settings and generate fuzzy objects
  //now just sustitle it with randomly generated number
  if (coolDownOnceTrigger ==0){
    coolDownOnce = millis()+onceDelay;
    coolDownOnceTrigger = 1;
  }
  if (millis() > coolDownOnce){  
    processButtons();
    coolDownOnceTrigger = 0;
  }
 
  if (newTemperature != oldTemperature){
    deleteFuzzy();
    setUpFuzzy(newTemperature);  
    oldTemperature = newTemperature;
    Serial.print("Create Fuzzy");
    Serial.println();
  }
 
  int currentTemp = ((analogRead(temperatureInputPin)*0.6)+20);
  fuzzy->setInput(1,currentTemp);
  fuzzy->fuzzify();
  float pwmThrottle = fuzzy->defuzzify(1);
    
  Serial.print("New temperature -> ");
  Serial.print(newTemperature);
  Serial.println();
  Serial.print("Action - > ");
  Serial.print(pwmThrottle);
  Serial.println();
  analogWrite(pwnOutputPin,pwmThrottle);
  Serial.print("Thermal Readout? -> ");
  Serial.print(currentTemp);
  Serial.println();
 
  delay(10);
 
}
