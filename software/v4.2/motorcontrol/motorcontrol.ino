 //Libraries
#include "Stepper.h"
#include "CommandHandler.h"
#include "MegunoLink.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"

//Agitation Variables
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution for motors
int stepCount = 0;  // number of steps the motor has taken
int AgitationSpd = 2;// 
int Agitation = 0;
int AgLight = 0;
int stepperPin =7;
// initialize the stepper library for agitation  :
//Agitation pins are from 22 to 45
Stepper Stepper1(stepsPerRevolution, 22, 24, 26, 28);
Stepper Stepper2(stepsPerRevolution, 23, 25, 27, 29);
Stepper Stepper3(stepsPerRevolution, 30, 32, 34, 36);
Stepper Stepper4(stepsPerRevolution, 31, 33, 35, 37);
Stepper Stepper5(stepsPerRevolution, 38, 40, 42, 44);
Stepper Stepper6(stepsPerRevolution, 46, 48, 50, 52);

InterfacePanel Panel("Control") ;  // Channel used in MLPUI Message processor in interface Panel
InterfacePanel Panel2("Settings");
//Megunolink Communications
CommandHandler<> SerialCommandHandler;

//Megunolink Functions
void SetAgOn(CommandParameter &Parameters)           
{
  Agitation= 1;
  AgLight =1;
  Panel.SetProgress("AgLight",AgLight);
}
void SetAgOff(CommandParameter &Parameters)           
{
  Agitation= 0;
  AgLight =0;
  Panel.SetProgress("AgLight",AgLight);
}

void SetAgSpd(CommandParameter &Parameters)           
{
  AgitationSpd=Parameters.NextParameterAsInteger();
}

void DoUnknown()
{
  Serial.println("I don't understand!");                        
}

void setup() {
  // put your setup code here, to run once:
  // Set baud rate
  Serial.begin(9600);
  //Turn Panel Light Off
  Panel.SetProgress("AgLight",AgLight);

   SerialCommandHandler.AddCommand(F("AgOn"), SetAgOn);
 SerialCommandHandler.AddCommand(F("AgOff"), SetAgOff);
 SerialCommandHandler.AddCommand(F("SetSpd"), SetAgSpd);
 SerialCommandHandler.SetDefaultHandler(DoUnknown);
}

void loop() {
  // put your main code here, to run repeatedly:
SerialCommandHandler.Process();
digitalWrite(stepperPin, LOW);
// Agitation Control
  if (Agitation == 1)  {
//    Serial.println("SetupOver");
    // step a revolution:
 //   Serial.println("Agitation");
    Stepper1.step(1);
    Stepper2.step(1);
    Stepper3.step(1);
    Stepper4.step(1);
    Stepper5.step(1);
    Stepper6.step(1);
    delayMicroseconds(1500+1000*AgitationSpd);
  //  Serial.println("Agitation");
      
  }
  else 
  {
    SerialCommandHandler.Process();
  }
}
