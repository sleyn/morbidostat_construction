
 //Libraries
#include "Stepper.h"
#include "CommandHandler.h"
#include "MegunoLink.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"
#include "math.h"
//Defining Variables

//Agitation Variables
const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution for motors
int stepCount = 0;  // number of steps the motor has taken
int AgitationSpd = 2;// 
int Agitation = 0;
int AgLight = 0;

// Tube choice variables
int tube1 = 0;
int tube2 = 0;
int tube3 = 0;
int tube4 = 0;
int tube5 = 0;
int tube6 = 0;

// Temperature Variables
int TempStatus = 0;
int TempLight = 0;
int setTemp = 37;
double temp = 20;
int heater = 2;    //heating pin
#define ONE_WIRE_BUS 3 // temperature sensor pin
unsigned long previousTempMillis = 0; 
const long tempInterval = 10000; 
float Min = 100;
float Max = 0;
//Temp Sensor Callup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//Turbidity Reading Variables
int laserPin1 = 13; // Laser relay pin
int laserPin2 = 12;
int laserPin3 = 11;
int laserPin4 = 10;
int laserPin5 = 9;
int laserPin6 = 8;

//Calibration variables
float C1 = -0.75;
float C2 = -0.81;
float C3 = -0.65;
float C4 = -0.67;
float C5 = -0.60;
float C6 = -0.64;

float V01 = 3.44;
float V02 = 3.71;
float V03 = 3.16;
float V04 = 3.10;
float V05 = 2.90;
float V06 = 3.08;

int Laser1 = 15; // Laser Strength
int Laser2 = 15;
int Laser3 = 15;
int Laser4 = 15;
int Laser5 = 15;
int Laser6 = 15;
int ODStatus = 0; // Readings on or off
int ODLight = 0;
float thrs1 = 0.4; // threshold to decide which pump is used
float thrs2 = 0.4;
float thrs3 = 0.4;
float thrs4 = 0.4;
float thrs5 = 0.4;
float thrs6 = 0.4;

float minthrs1 = 0.25; // minimum threshold to trigger dilution
float minthrs2 = 0.25;
float minthrs3 = 0.25;
float minthrs4 = 0.25;
float minthrs5 = 0.25;
float minthrs6 = 0.25;
unsigned long previousDilMillis = 0; 
const long DilInterval = 5000;  // interval between OD readings in milliseconds
double OD1 = 0.0; // Turbidity reading from tube 1
double OD2 = 0.0;
double OD3 = 0.0;
double OD4 = 0.0;
double OD5 = 0.0;
double OD6 = 0.0;

//Dilution Variables
int DilStatus = 0;
int AutDilLight = 0;
int AirPump = 52; // Air pump PIN check if correct in dilution function and beginning of loop!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int AirDelay = 5; // time air pump is off after a dilution
int Air = 0;
unsigned long previousDilutionTime1 = 0; 
unsigned long previousDilutionTime2 = 0; 
unsigned long previousDilutionTime3 = 0; 
unsigned long previousDilutionTime4 = 0; 
unsigned long previousDilutionTime5 = 0; 
unsigned long previousDilutionTime6 = 0; 

double previousOD1 = 0.0; // Turbidity reading from tube 1
double previousOD2 = 0.0;
double previousOD3 = 0.0;
double previousOD4 = 0.0;
double previousOD5 = 0.0;
double previousOD6 = 0.0;

int CTime1 = 15; // (Cycle Time) Time between dilutions (mn)
int CTime2 = 15;
int CTime3 = 15;
int CTime4 = 15;
int CTime5 = 15;
int CTime6 = 15;

int DTime1 = 16; // Time pump runs during dilution (seconds)
int DTime2 = 16;
int DTime3 = 16;
int DTime4 = 16;
int DTime5 = 16;
int DTime6 = 16;

int PumpSpd = 100;

int pump1 = 4;       //pump w/o antibiotics
int pump2 = 5;       //pump with antibiotics

int Tube1Pump = 1;  // for specifying which pump is on during dilutions in the messages
int Tube2Pump = 1;
int Tube3Pump = 1;
int Tube4Pump = 1;
int Tube5Pump = 1;
int Tube6Pump = 1;

// Servo Control is using adafruit 16-Channel 12-bit PWM/Servo Driver - I2C interface - PCA9685
int valve1 = 0;
int valve2 = 1;
int valve3 = 2;
int valve4 = 3;
int valve5 = 4;
int valve6 = 5;

// Cleaning
int Cleaning = 0;

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  195 // this is the 'minimum' pulse length count (out of 4096) at this pulse lenght valve is closed
#define SERVOMAX  400 // this is the 'maximum' pulse length count (out of 4096) at this pulse lenght valve is open


//Plots and Messages in Megunolink
TimePlot MyPlot;
Message MyCSVMessage("Data"); //"Data" = the target message channel (remember to select this in megunolink)
Message PumpUsage("Pumps"); //What pump was used
InterfacePanel Panel("Control") ;  // Channel used in MLPUI Message processor in interface Panel
InterfacePanel Panel2("Settings");
InterfacePanel Panel3("Calibration");
//Megunolink Communications
CommandHandler<> SerialCommandHandler;

//Megunolink Functions

// Set length of time air pump is off during dilution
void SetAirDelay(CommandParameter &Parameters)           
{
  AirDelay =Parameters.NextParameterAsInteger();
}
// Set laser strengths
void SetLaser1(CommandParameter &Parameters)           
{
  Laser1 =Parameters.NextParameterAsInteger();
}

void SetLaser2(CommandParameter &Parameters)           
{
  Laser2 =Parameters.NextParameterAsInteger();
}

void SetLaser3(CommandParameter &Parameters)           
{
  Laser3 =Parameters.NextParameterAsInteger();
}

void SetLaser4(CommandParameter &Parameters)           
{
  Laser4 =Parameters.NextParameterAsInteger();
}
void SetLaser5(CommandParameter &Parameters)           
{
  Laser5 =Parameters.NextParameterAsInteger();
}
void SetLaser6(CommandParameter &Parameters)           
{
  Laser6 =Parameters.NextParameterAsInteger();
}

//Set calibration functions
void SetC1(CommandParameter &Parameters)           
{
  C1 =Parameters.NextParameterAsDouble();
}
void SetC2(CommandParameter &Parameters)           
{
  C2 =Parameters.NextParameterAsDouble();
}
void SetC3(CommandParameter &Parameters)           
{
  C3 =Parameters.NextParameterAsDouble();
}
void SetC4(CommandParameter &Parameters)           
{
  C4 =Parameters.NextParameterAsDouble();
}
void SetC5(CommandParameter &Parameters)           
{
  C5 =Parameters.NextParameterAsDouble();
}
void SetC6(CommandParameter &Parameters)           
{
  C6 =Parameters.NextParameterAsDouble();
}
void SetV01(CommandParameter &Parameters)           
{
  V01 =Parameters.NextParameterAsDouble();
}
void SetV02(CommandParameter &Parameters)           
{
  V02 =Parameters.NextParameterAsDouble();
}
void SetV03(CommandParameter &Parameters)           
{
  V03 =Parameters.NextParameterAsDouble();
}
void SetV04(CommandParameter &Parameters)           
{
  V04 =Parameters.NextParameterAsDouble();
}
void SetV05(CommandParameter &Parameters)           
{
  V05 =Parameters.NextParameterAsDouble();
}
void SetV06(CommandParameter &Parameters)           
{
  V06 =Parameters.NextParameterAsDouble();
}



// Setting "controls" panel function
void SetODOn(CommandParameter &Parameters)           
{
  ODStatus= 1;
  ODLight =1;
  Panel.SetProgress("ODLight",ODLight);
}
void SetODOff(CommandParameter &Parameters)           
{
  ODStatus= 0;
  ODLight =0;
  Panel.SetProgress("ODLight",ODLight);
}
void SetColors(CommandParameter &Parameters)           
{
  MyPlot.SetTitle("Turbidimeter");
  MyPlot.SetXlabel("Time");
  MyPlot.SetYlabel("OD");
  MyPlot.SetSeriesProperties("Tube1-P1", Plot::Magenta, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube1-P2", Plot::Magenta, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube2-P1", Plot::Green, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube2-P1", Plot::Green, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube3-P1", Plot::Blue, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube3-P2", Plot::Blue, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube4-P1", Plot::Red, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube4-P2", Plot::Red, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube5-P1", Plot::Yellow, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube5-P2", Plot::Yellow, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube6-P1", Plot::Black, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube6-P2", Plot::Black, Plot::Solid, 2, Plot::Square);
}
void SetAutDilOn(CommandParameter &Parameters)           
{
  DilStatus= 1;
  AutDilLight =1;
  Panel.SetProgress("AutDilLight",AutDilLight);
}
void SetAutDilOff(CommandParameter &Parameters)           
{
  DilStatus= 0;
  AutDilLight =0;
  Panel.SetProgress("AutDilLight",AutDilLight);
}
void SetTempOn(CommandParameter &Parameters)           
{
  TempStatus= 1;
  TempLight =1;
  Panel.SetProgress("TempLight",TempLight);
}
void SetTempOff(CommandParameter &Parameters)           
{
  TempStatus= 0;
  TempLight =0;
  Panel.SetProgress("TempLight",TempLight);
}

void Cmd_SetTemp(CommandParameter &Parameters)
{
  setTemp=Parameters.NextParameterAsInteger();
}
void DoTempMin(CommandParameter &Parameters)
{
  Min = 100;
  Panel.SetNumber("MinTemp", Min);
}

void DoTempMax(CommandParameter &Parameters)
{
  Max = 0;
  Panel.SetNumber("MaxTemp", Max);
}

void SetTube1(CommandParameter &Parameters)           
{
  
  tube1 = Parameters.NextParameterAsInteger();
}

void SetTube2(CommandParameter &Parameters)           
{
  
  tube2 = Parameters.NextParameterAsInteger();
}

void SetTube3(CommandParameter &Parameters)           
{
  
  tube3 = Parameters.NextParameterAsInteger();
}

void SetTube4(CommandParameter &Parameters)           
{
  
  tube4 = Parameters.NextParameterAsInteger();
}

void SetTube5(CommandParameter &Parameters)           
{
  
  tube5 = Parameters.NextParameterAsInteger();
}

void SetTube6(CommandParameter &Parameters)           
{
  
  tube6 = Parameters.NextParameterAsInteger();
}
//Megunolink Functions for Settings
void SetThrs1(CommandParameter &Parameters)           
{
  thrs1 = Parameters.NextParameterAsDouble();
  Serial.println(thrs1);
}

void SetThrs2(CommandParameter &Parameters)           
{
  thrs2 = Parameters.NextParameterAsDouble();
  Serial.println(thrs2);
}

void SetThrs3(CommandParameter &Parameters)           
{
  thrs3 = Parameters.NextParameterAsDouble();
  Serial.println(thrs3);
}

void SetThrs4(CommandParameter &Parameters)           
{
  thrs4 = Parameters.NextParameterAsDouble();
  Serial.println(thrs4);
}

void SetThrs5(CommandParameter &Parameters)           
{
  thrs5 = Parameters.NextParameterAsDouble();
  Serial.println(thrs5);
}

void SetThrs6(CommandParameter &Parameters)           
{
  thrs6 = Parameters.NextParameterAsDouble();
  Serial.println(thrs6);
}
// Set miniumum thresholds
void SetLThrs1(CommandParameter &Parameters)           
{
  minthrs1 = Parameters.NextParameterAsDouble();
  Serial.println(minthrs1);
}

void SetLThrs2(CommandParameter &Parameters)           
{
  minthrs2 = Parameters.NextParameterAsDouble();
  Serial.println(minthrs2);
}

void SetLThrs3(CommandParameter &Parameters)           
{
  minthrs3 = Parameters.NextParameterAsDouble();
  Serial.println(minthrs3);
}

void SetLThrs4(CommandParameter &Parameters)           
{
  minthrs4 = Parameters.NextParameterAsDouble();
  Serial.println(minthrs4);
}

void SetLThrs5(CommandParameter &Parameters)           
{
  minthrs5 = Parameters.NextParameterAsDouble();
  Serial.println(minthrs5);
}

void SetLThrs6(CommandParameter &Parameters)           
{
  minthrs6 = Parameters.NextParameterAsDouble();
  Serial.println(minthrs6);
}
// Set Cycle Time Functions
void SetCTime1(CommandParameter &Parameters)           
{
  CTime1 = Parameters.NextParameterAsInteger();
  Serial.println(CTime1);
}

void SetCTime2(CommandParameter &Parameters)           
{
  CTime2 = Parameters.NextParameterAsInteger();
  Serial.println(CTime2);
}

void SetCTime3(CommandParameter &Parameters)           
{
  CTime3 = Parameters.NextParameterAsInteger();
  Serial.println(CTime3);
}

void SetCTime4(CommandParameter &Parameters)           
{
  CTime4 = Parameters.NextParameterAsInteger();
  Serial.println(CTime4);
}

void SetCTime5(CommandParameter &Parameters)           
{
  CTime5 = Parameters.NextParameterAsInteger();
  Serial.println(CTime5);
}

void SetCTime6(CommandParameter &Parameters)           
{
  CTime6 = Parameters.NextParameterAsInteger();
  Serial.println(CTime6);
}
// Set Dilution Time functions

void SetDTime1(CommandParameter &Parameters)           
{
  DTime1 = Parameters.NextParameterAsInteger();
  Serial.println(DTime1);
}

void SetDTime2(CommandParameter &Parameters)           
{
  DTime2 = Parameters.NextParameterAsInteger();
  Serial.println(DTime2);
}

void SetDTime3(CommandParameter &Parameters)           
{
  DTime3 = Parameters.NextParameterAsInteger();
  Serial.println(DTime3);
}

void SetDTime4(CommandParameter &Parameters)           
{
  DTime4 = Parameters.NextParameterAsInteger();
  Serial.println(DTime4);
}

void SetDTime5(CommandParameter &Parameters)           
{
  DTime5 = Parameters.NextParameterAsInteger();
  Serial.println(DTime5);
}

void SetDTime6(CommandParameter &Parameters)           
{
  DTime6 = Parameters.NextParameterAsInteger();
  Serial.println(DTime6);
}

//Set Pump Spd
void SetPumpSpd(CommandParameter &Parameters)           
{
  PumpSpd = Parameters.NextParameterAsInteger();
  Serial.println(PumpSpd);
}
// Megunolink Functions for Manual Overrides

void SetAir(CommandParameter &Parameters)    //turn on/off air pump
{
  //Serial.println("Air");
  Air = Parameters.NextParameterAsInteger();
  if (Air ==1) {
    digitalWrite(AirPump,HIGH);
  }
  else {
    digitalWrite(AirPump,LOW);
  }
}

void SetPump1(CommandParameter &Parameters)           
{
  Serial.println("Pump1");
  int Pump01 = Parameters.NextParameterAsInteger();
  if (Pump01 ==1) {
    analogWrite(pump1,255);
  }
  else {
    analogWrite(pump1,LOW);
  }
}
void SetPump2(CommandParameter &Parameters)           
{
  int Pump02 = Parameters.NextParameterAsInteger();
  if (Pump02 ==1) {
    analogWrite(pump2,255);
  }
  else {
    analogWrite(pump2,LOW);
  }
}
//////Manual Overrides for Valves
//Valve1
void SetValve1(CommandParameter &Parameters)           
{
  int Valve01 = Parameters.NextParameterAsInteger();
  if (Valve01 ==1) 
  {
      for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) 
      {
      pwm.setPWM(valve1, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve1,0,0);
  }
  else 
  {
      for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
      {
      pwm.setPWM(valve1, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve1,0,0);
  }
}
//Valve2
void SetValve2(CommandParameter &Parameters)           
{
  int Valve02 = Parameters.NextParameterAsInteger();
  if (Valve02 ==1) 
  {
      for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) 
      {
      pwm.setPWM(valve2, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve2,0,0);
  }
  else 
  {
      for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
      {
      pwm.setPWM(valve2, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve2,0,0);
  }
}
//Valve3
void SetValve3(CommandParameter &Parameters)           
{
  int Valve03 = Parameters.NextParameterAsInteger();
  if (Valve03 ==1) 
  {
      for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) 
      {
      pwm.setPWM(valve3, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve3,0,0);
  }
  else 
  {
      for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
      {
      pwm.setPWM(valve3, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve3,0,0);
  }
}
//Valve4
void SetValve4(CommandParameter &Parameters)           
{
  int Valve04 = Parameters.NextParameterAsInteger();
  if (Valve04 ==1) 
  {
      for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) 
      {
      pwm.setPWM(valve4, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve4,0,0);
  }
  else 
  {
      for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
      {
      pwm.setPWM(valve4, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve4,0,0);
  }
}
//Valve5
void SetValve5(CommandParameter &Parameters)           
{
  int Valve05 = Parameters.NextParameterAsInteger();
  if (Valve05 ==1) 
  {
      for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) 
      {
      pwm.setPWM(valve5, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve5,0,0);
  }
  else 
  {
      for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
      {
      pwm.setPWM(valve5, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve5,0,0);
  }
}
//Valve6
void SetValve6(CommandParameter &Parameters)           
{
  int Valve06 = Parameters.NextParameterAsInteger();
  if (Valve06 ==1) 
  {
      for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) 
      {
      pwm.setPWM(valve6, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve6,0,0);
  }
  else 
  {
      for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) 
      {
      pwm.setPWM(valve6, 0, pulselen);
      }
      delay(300);
      pwm.setPWM(valve6,0,0);
  }
}

void SetCleaning1(CommandParameter &Parameters)           
{
  int Cleaning1 = Parameters.NextParameterAsInteger();
  if (Cleaning1 ==1) {
    Cleaning = 0;
  }
  else {
    delay(1);
  }
}


void SetCleaning2(CommandParameter &Parameters)           
{
  int Cleaning2 = Parameters.NextParameterAsInteger();
  if (Cleaning2 ==1) {
    Cleaning = 1;
  }
  else {
    delay(1);
  }
}

void SetCleaning3(CommandParameter &Parameters)           
{
  int Cleaning3 = Parameters.NextParameterAsInteger();
  if (Cleaning3 ==1) {
    Cleaning = 2;
  }
  else {
    delay(1);
  }
}


void DoUnknown()
{
  Serial.println("I don't understand!");                        
}

//Dilution Function
void Dilution(int Pump_Spd, int Dilution_Time,int pump_1,int valve_1,int air_delay)
{ 
  digitalWrite(heater, LOW); //turn heat off
  if( air_delay >= 0 )    //air_delay == -1 means that air delay should not be implemented
  {
    digitalWrite(AirPump,LOW); // turn air pump off at pin 52 check this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  }
  //open valve_1
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++) {
  pwm.setPWM(valve_1, 0, pulselen);
  }
  delay(300);
  pwm.setPWM(valve_1, 0, 0);

  // PWM pump using Ratio
  analogWrite(pump_1,(Pump_Spd*255/100));
  //Wait
  delay(Dilution_Time*1000);
  //shut down pumps
  analogWrite(pump_1,0);
  // close valve_1
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--) {
  pwm.setPWM(valve_1, 0, pulselen);
  }
  delay(300);
  pwm.setPWM(valve_1, 0, 0);

  if( air_delay >= 0 && Air == 1)
  {
    delay(1000*air_delay);
    digitalWrite(AirPump,HIGH); // turn air pump back on at pin 52 check this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  }
}

// Running Average Function

void setup() {
  // Set baud rate
  Serial.begin(9600);

  PumpUsage.Begin();    //Add
  Serial.print("Initialization");    //Add
  PumpUsage.End();    //Add
  
  //Start temperature Sensor
  sensors.begin();
  // Set Heater pin as output
  pinMode(heater,OUTPUT);
  pinMode(pump1,OUTPUT);
  pinMode(pump2,OUTPUT);
  pinMode(laserPin1,OUTPUT);
  pinMode(laserPin2,OUTPUT);
  pinMode(laserPin3,OUTPUT);
  pinMode(laserPin4,OUTPUT);
  pinMode(laserPin5,OUTPUT);
  pinMode(laserPin6,OUTPUT);
  pinMode(AirPump,OUTPUT);
  
  Panel.SetProgress("ODLight",ODLight);
  Panel.SetProgress("AutDilLight",AutDilLight);
  Panel.SetProgress("TempLight",TempLight);
  Panel.SetNumber("SetTemp",setTemp);
  tube1 = 0;
  tube2 = 0;
  tube3 = 0;
  tube4 = 0;
  tube5 = 0;
  tube6 = 0;
  Panel.SetCheck("Tube1", tube1);
  Panel.SetCheck("Tube2", tube2);
  Panel.SetCheck("Tube3", tube3);
  Panel.SetCheck("Tube4", tube4);
  Panel.SetCheck("Tube5", tube5);
  Panel.SetCheck("Tube6", tube6);
  analogWrite(laserPin1,0);
  analogWrite(laserPin2,0);
  analogWrite(laserPin3,0);
  analogWrite(laserPin4,0);
  analogWrite(laserPin5,0);
  analogWrite(laserPin6,0);
// Display all original Settings
//  int intthrs1 = thrs1;
  Panel2.SetNumber("Thrs1", thrs1);
//  int intthrs2 = thrs2;
  Panel2.SetNumber("Thrs2", thrs2);
//  int intthrs3 = thrs3;
  Panel2.SetNumber("Thrs3", thrs3);
//  int intthrs4 = thrs4;
  Panel2.SetNumber("Thrs4", thrs4);
//  int intthrs5 = thrs5;
  Panel2.SetNumber("Thrs5", thrs5);
//  int intthrs6 = thrs6;
  Panel2.SetNumber("Thrs6", thrs6);
  
  int Valve01 = 0;
  int Valve02 = 0;
  int Valve03 = 0;
  int Valve04 = 0;
  int Valve05 = 0;
  int Valve06 = 0;
  Panel2.SetCheck("Valve1", Valve01);
  Panel2.SetCheck("Valve2", Valve02);
  Panel2.SetCheck("Valve3", Valve03);
  Panel2.SetCheck("Valve4", Valve04);
  Panel2.SetCheck("Valve5", Valve05);
  Panel2.SetCheck("Valve6", Valve06);
  
  int Pump01 = 0;
  int Pump02 = 0;
  Panel2.SetCheck("Pump1", Pump01);
  Panel2.SetCheck("Pump2", Pump02);
  Panel2.SetCheck("Air", Air);
  Panel2.SetNumber("LThrs1", minthrs1);
  Panel2.SetNumber("LThrs2", minthrs2);
  Panel2.SetNumber("LThrs3", minthrs3);
  Panel2.SetNumber("LThrs4", minthrs4);
  Panel2.SetNumber("LThrs5", minthrs5);
  Panel2.SetNumber("LThrs6", minthrs6);  
  
  Panel2.SetNumber("CTime1", CTime1);
  Panel2.SetNumber("CTime2", CTime2);
  Panel2.SetNumber("CTime3", CTime3);
  Panel2.SetNumber("CTime4", CTime4);
  Panel2.SetNumber("CTime5", CTime5);
  Panel2.SetNumber("CTime6", CTime6);  
  
  Panel2.SetNumber("DTime1", DTime1);
  Panel2.SetNumber("DTime2", DTime2);
  Panel2.SetNumber("DTime3", DTime3);
  Panel2.SetNumber("DTime4", DTime4);
  Panel2.SetNumber("DTime5", DTime5);
  Panel2.SetNumber("DTime6", DTime6);  
  
  Panel2.SetNumber("PumpSpd", PumpSpd);
  
  Panel2.SetNumber("Lsr1", Laser1);
  Panel2.SetNumber("Lsr2", Laser2);
  Panel2.SetNumber("Lsr3", Laser3);
  Panel2.SetNumber("Lsr4", Laser4);
  Panel2.SetNumber("Lsr5", Laser5);
  Panel2.SetNumber("Lsr6", Laser6);
  
  Panel3.SetNumber("C1", C1);
  Panel3.SetNumber("C2", C2);
  Panel3.SetNumber("C3", C3);
  Panel3.SetNumber("C4", C4);
  Panel3.SetNumber("C5", C5);
  Panel3.SetNumber("C6", C6);
  
  Panel3.SetNumber("V01", V01);
  Panel3.SetNumber("V02", V02);
  Panel3.SetNumber("V03", V03);
  Panel3.SetNumber("V04", V04);
  Panel3.SetNumber("V05", V05);
  Panel3.SetNumber("V06", V06);
  //Servo Stuff
  #ifdef ESP8266
  Wire.pins(13, 14);   // ESP8266 can use any two pins, such as SDA to #2 and SCL to #14
#endif

  pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
//yield() might create issues, check it when testing
 // yield();
  // Megunolink plot properties
  MyPlot.SetTitle("Turbidimeter");
  MyPlot.SetXlabel("Time");
  MyPlot.SetYlabel("OD");
  MyPlot.SetSeriesProperties("Tube1-P1", Plot::Magenta, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube1-P2", Plot::Magenta, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube2-P1", Plot::Green, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube2-P1", Plot::Green, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube3-P1", Plot::Blue, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube3-P2", Plot::Blue, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube4-P1", Plot::Red, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube4-P2", Plot::Red, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube5-P1", Plot::Yellow, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube5-P2", Plot::Yellow, Plot::Solid, 2, Plot::Square);
  MyPlot.SetSeriesProperties("Tube6-P1", Plot::Black, Plot::Solid, 2, Plot::Star);
  MyPlot.SetSeriesProperties("Tube6-P2", Plot::Black, Plot::Solid, 2, Plot::Square);
  
 //Megunolink Functions Callups (Main Controls)


 SerialCommandHandler.AddCommand(F("ODOn"), SetODOn);
 SerialCommandHandler.AddCommand(F("ODOff"), SetODOff);
 SerialCommandHandler.AddCommand(F("ColRes"), SetColors);
 

 SerialCommandHandler.AddCommand(F("AutDilOn"), SetAutDilOn);
 SerialCommandHandler.AddCommand(F("AutDilOff"), SetAutDilOff);
 SerialCommandHandler.AddCommand(F("TempOff"), SetTempOff);
 SerialCommandHandler.AddCommand(F("TempOn"), SetTempOn);
 SerialCommandHandler.AddCommand(F("SetTemp"), Cmd_SetTemp); // megunolink
 SerialCommandHandler.AddCommand(F("TempMin"), DoTempMin);
 SerialCommandHandler.AddCommand(F("TempMax"), DoTempMax);
 SerialCommandHandler.AddCommand(F("Tube1"), SetTube1);
 SerialCommandHandler.AddCommand(F("Tube2"), SetTube2);
 SerialCommandHandler.AddCommand(F("Tube3"), SetTube3);
 SerialCommandHandler.AddCommand(F("Tube4"), SetTube4);
 SerialCommandHandler.AddCommand(F("Tube5"), SetTube5);
 SerialCommandHandler.AddCommand(F("Tube6"), SetTube6);
 
 //Megunolink Functions Callups (Settings)
 SerialCommandHandler.AddCommand(F("AirDelay"), SetAirDelay);
 
 SerialCommandHandler.AddCommand(F("Thrs1"), SetThrs1); 
 SerialCommandHandler.AddCommand(F("Thrs2"), SetThrs2);
 SerialCommandHandler.AddCommand(F("Thrs3"), SetThrs3);
 SerialCommandHandler.AddCommand(F("Thrs4"), SetThrs4);
 SerialCommandHandler.AddCommand(F("Thrs5"), SetThrs5);
 SerialCommandHandler.AddCommand(F("Thrs6"), SetThrs6);
 
 SerialCommandHandler.AddCommand(F("LThrs1"), SetLThrs1); //lower thresholds
 SerialCommandHandler.AddCommand(F("LThrs2"), SetLThrs2);
 SerialCommandHandler.AddCommand(F("LThrs3"), SetLThrs3);
 SerialCommandHandler.AddCommand(F("LThrs4"), SetLThrs4);
 SerialCommandHandler.AddCommand(F("LThrs5"), SetLThrs5);
 SerialCommandHandler.AddCommand(F("LThrs6"), SetLThrs6);
 
 SerialCommandHandler.AddCommand(F("CTime1"), SetCTime1); 
 SerialCommandHandler.AddCommand(F("CTime2"), SetCTime2); 
 SerialCommandHandler.AddCommand(F("CTime3"), SetCTime3); 
 SerialCommandHandler.AddCommand(F("CTime4"), SetCTime4); 
 SerialCommandHandler.AddCommand(F("CTime5"), SetCTime5); 
 SerialCommandHandler.AddCommand(F("CTime6"), SetCTime6); 
 
 SerialCommandHandler.AddCommand(F("DTime1"), SetDTime1); 
 SerialCommandHandler.AddCommand(F("DTime2"), SetDTime2); 
 SerialCommandHandler.AddCommand(F("DTime3"), SetDTime3); 
 SerialCommandHandler.AddCommand(F("DTime4"), SetDTime4); 
 SerialCommandHandler.AddCommand(F("DTime5"), SetDTime5); 
 SerialCommandHandler.AddCommand(F("DTime6"), SetDTime6); 
 
 SerialCommandHandler.AddCommand(F("Lsr1"), SetLaser1);
 SerialCommandHandler.AddCommand(F("Lsr2"), SetLaser2);
 SerialCommandHandler.AddCommand(F("Lsr3"), SetLaser3);
 SerialCommandHandler.AddCommand(F("Lsr4"), SetLaser4);
 SerialCommandHandler.AddCommand(F("Lsr5"), SetLaser5);
 SerialCommandHandler.AddCommand(F("Lsr6"), SetLaser6);
 
 SerialCommandHandler.AddCommand(F("PumpSpd"), SetPumpSpd); 
 //Megunolink Functions Callups (Manual Controls)
 SerialCommandHandler.AddCommand(F("Pump1"), SetPump1);
 SerialCommandHandler.AddCommand(F("Pump2"), SetPump2);
 SerialCommandHandler.AddCommand(F("Air"), SetAir);
 SerialCommandHandler.AddCommand(F("Valve1"), SetValve1);
 SerialCommandHandler.AddCommand(F("Valve2"), SetValve2);
 SerialCommandHandler.AddCommand(F("Valve3"), SetValve3);
 SerialCommandHandler.AddCommand(F("Valve4"), SetValve4);
 SerialCommandHandler.AddCommand(F("Valve5"), SetValve5);
 SerialCommandHandler.AddCommand(F("Valve6"), SetValve6);
 //Cleaning Controls
 SerialCommandHandler.AddCommand(F("Cleaning1"), SetCleaning1);
 SerialCommandHandler.AddCommand(F("Cleaning2"), SetCleaning2);
 SerialCommandHandler.AddCommand(F("Cleaning3"), SetCleaning3);
 //Calibration Controls
 SerialCommandHandler.AddCommand(F("C1"), SetC1);
 SerialCommandHandler.AddCommand(F("C2"), SetC2);
 SerialCommandHandler.AddCommand(F("C3"), SetC3);
 SerialCommandHandler.AddCommand(F("C4"), SetC4);
 SerialCommandHandler.AddCommand(F("C5"), SetC5);
 SerialCommandHandler.AddCommand(F("C6"), SetC6);
 SerialCommandHandler.AddCommand(F("V01"), SetV01);
 SerialCommandHandler.AddCommand(F("V02"), SetV02);
 SerialCommandHandler.AddCommand(F("V03"), SetV03);
 SerialCommandHandler.AddCommand(F("V04"), SetV04);
 SerialCommandHandler.AddCommand(F("V05"), SetV05);
 SerialCommandHandler.AddCommand(F("V06"), SetV06);
 
 
 SerialCommandHandler.SetDefaultHandler(DoUnknown);
}

void loop() {
SerialCommandHandler.Process();
  
  //digitalWrite(AirPump,HIGH); // turn air pump on !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  delay(50);
 SerialCommandHandler.Process();
  
 // Temperature Control using Dallas Temperature Library
  unsigned long currentTempMillis = millis();
  if(currentTempMillis - previousTempMillis >= tempInterval) 
  {
    previousTempMillis = currentTempMillis;     // save the last time you checked Temperature 
    if (TempStatus == 1) 
   { 
      Serial.println("Temperature");
      sensors.requestTemperatures(); // Send the command to get temperatures
      temp = sensors.getTempCByIndex(0);
      if (temp < setTemp)
      {
        digitalWrite(heater, HIGH);
      }
      else
      {
        digitalWrite(heater, LOW);
      }
   }
   else
   {
     digitalWrite(heater, LOW);
   }
   sensors.requestTemperatures();         
   float temp = sensors.getTempCByIndex(0);    
         if (temp >= Max)
         {
           Max = temp;
         }
         if (temp <= Min)
         {
           Min = temp;
         }
   Panel.SetNumber("MinTemp", Min);
   Panel.SetNumber("MaxTemp", Max);
   Panel.SetNumber("CurrentTemp",temp); 
  }//Time to Read temperature
  
  //Turbidity Readings
  
  // if it is time to read

    unsigned long currentDilMillis = millis();
    if(currentDilMillis - previousDilMillis >= DilInterval) 
    { 
      previousDilMillis = currentDilMillis;
   // if turbity readings button is on -- read
      if (ODStatus == 1)
      {  
       analogWrite(laserPin1,Laser1);// turn on lasers 
       analogWrite(laserPin2,Laser2); 
       analogWrite(laserPin3,Laser3);
       analogWrite(laserPin4,Laser4);
       analogWrite(laserPin5,Laser5);
       analogWrite(laserPin6,Laser6);
       
        delay(1000);
        double Od1 = 0.0;
        double Od2 = 0.0;
        double Od3 = 0.0;
        double Od4 = 0.0;
        double Od5 = 0.0;
        double Od6 = 0.0;
        double Od7 = 0.0;
        double Od8 = 0.0;
        double Od9 = 0.0;
        double Od10 = 0.0;
        double Od11 = 0.0;
        double Od12 = 0.0;
        for (int k=0;k<300;k=k+1) 
          {
            int P0 = analogRead(A0);  // Read photodiodes
            int P1 = analogRead(A1);
            int P2 = analogRead(A2);
            int P3 = analogRead(A3);
            int P4 = analogRead(A4);
            int P5 = analogRead(A5);
            int P6 = analogRead(A12);
            int P7 = analogRead(A13);
            int P8 = analogRead(A8);
            int P9 = analogRead(A9);
            int P10 = analogRead(A10);
            int P11 = analogRead(A11);
            
            Od1 = Od1 + (P0);  // analog read and divide
            Od2 = Od2 + (P1); 
            Od3 = Od3 + (P2); 
            Od4 = Od4 + (P3); 
            Od5 = Od5 + (P4); 
            Od6 = Od6 + (P5); 
            Od7 = Od7 + (P6); 
            Od8 = Od8 + (P7); 
            Od9 = Od9 + (P8); 
            Od10 = Od10 +(P9); 
            Od11 = Od11 +(P10); 
            Od12 = Od12 +(P11); 
            delay(1);
          }
          
        OD1 = C1*log10(Od2)+V01;
        OD2 = C2*log10(Od3)+V02;
        OD3 = C3*log10(Od6)+V03;
        OD4 = C4*log10(Od8)+V04;
        OD5 = C5*log10(Od10)+V05;
        OD6 = C6*log10(Od12)+V06;
        
        analogWrite(laserPin1,0); // turn off lasers
        analogWrite(laserPin2,0);
        analogWrite(laserPin3,0);
        analogWrite(laserPin4,0);
        analogWrite(laserPin5,0);
        analogWrite(laserPin6,0);

    // send to graph and to message if tube # is checked in settings
    if (tube1 == 1)
    {
       
      MyCSVMessage.Begin();  //Message for tube 1   
      Serial.print("Tube1");
      Serial.print(",");
      Serial.print(OD1);
      Serial.print(",");
      if (Tube1Pump == 1)
      {
        Serial.print("Pump1");
        MyCSVMessage.End();
        MyPlot.SendData("Tube1-P1",(OD1)); 
      }
      else 
      {
        Serial.print("Pump2");
        MyCSVMessage.End();
        MyPlot.SendData("Tube1-P2",(OD1));
      }
       
    }
    if (tube2 == 1)
    {  
      MyCSVMessage.Begin();  //Message for tube 2
      Serial.print("Tube2");
      Serial.print(",");
      Serial.print(OD2);
      Serial.print(",");
      if (Tube2Pump == 1)
      {
        Serial.print("Pump1");
        MyCSVMessage.End();
        MyPlot.SendData("Tube2-P1",(OD2));
      }
      else 
      {
        Serial.print("Pump2");
        MyCSVMessage.End();
        MyPlot.SendData("Tube2-P2",(OD2));
      }
       
    }
    if (tube3 == 1)
    { 
      MyCSVMessage.Begin();  //Message for tube 3  
      Serial.print("Tube3");
      Serial.print(",");
      Serial.print(OD3);
      Serial.print(",");
      if (Tube3Pump == 1)
      {
        Serial.print("Pump1");
        MyCSVMessage.End();
        MyPlot.SendData("Tube3-P1",(OD3));
      }
      else 
      {
        Serial.print("Pump2");
        MyCSVMessage.End();
        MyPlot.SendData("Tube3-P2",(OD3));
      }
       
    }
    if (tube4 == 1)
    {
      MyCSVMessage.Begin();  //Message for tube 4
      Serial.print("Tube4");
      Serial.print(",");
      Serial.print(OD4);
      Serial.print(",");
      if (Tube4Pump == 1)
      {
        Serial.print("Pump1");
        MyCSVMessage.End();
        MyPlot.SendData("Tube4-P1",(OD4));
      }
      else 
      {
        Serial.print("Pump2");
        MyCSVMessage.End();
        MyPlot.SendData("Tube4-P2",(OD4));
      }
       
    }
    if (tube5 == 1)
    {
      
      MyCSVMessage.Begin();  //Message for tube 5
      Serial.print("Tube5");
      Serial.print(",");
      Serial.print(OD5);
      Serial.print(",");
      if (Tube5Pump == 1)
      {
        Serial.print("Pump1");
        MyCSVMessage.End();
        MyPlot.SendData("Tube5-P1",(OD5)); 
      }
      else 
      {
        Serial.print("Pump2");
        MyCSVMessage.End();
        MyPlot.SendData("Tube5-P2",(OD5));
      }
        
    }
    if (tube6 == 1)
    {
      
      MyCSVMessage.Begin();  //Message for tube 6 
      Serial.print("Tube6");
      Serial.print(",");
      Serial.print(OD6);
      Serial.print(",");
      if (Tube6Pump == 1)
      {
        Serial.print("Pump1");
        MyCSVMessage.End();
        MyPlot.SendData("Tube6-P1",(OD6));
      }
      else 
      {
        Serial.print("Pump2");
        MyCSVMessage.End();
        MyPlot.SendData("Tube6-P2",(OD6));
      }   
    }

  // Check to see if Automatic Dilution is ON    
      if (DilStatus == 1)
      {
          unsigned long currentDilutionTime1 = millis();
          unsigned long currentDilutionTime2 = millis();
          unsigned long currentDilutionTime3 = millis();
          unsigned long currentDilutionTime4 = millis();
          unsigned long currentDilutionTime5 = millis();
          unsigned long currentDilutionTime6 = millis();
         
          // If time to Dilute Tube1
          
         if(currentDilutionTime1 - previousDilutionTime1 >= CTime1*60000  && Cleaning == 0 && tube1 == 1 && OD1 >= minthrs1) 
         {
               
              previousDilutionTime1 = currentDilutionTime1;   // save Dilution Time 
              Serial.println("AutDil");
              if (OD1 >= thrs1 && OD1 >= previousOD1)
              { 
                  Dilution(PumpSpd,DTime1,pump2,valve1,AirDelay);  //Run dilution
                  Tube1Pump = 2;
                  PumpUsage.Begin();
                  Serial.print("Tube1 - Pump2");
                  PumpUsage.End();
              }
              else 
              {
                  Dilution(PumpSpd,DTime1,pump1,valve1,AirDelay);  //Run dilution
                  Tube1Pump = 1;
                  PumpUsage.Begin();
                  Serial.print("Tube1 - Pump1");
                  PumpUsage.End();
              }
              previousOD1 = OD1;
              

         }
         
         if (currentDilutionTime1 - previousDilutionTime1 >= CTime1*60000  && Cleaning == 1 && tube1 == 1) 
         {
               Dilution(PumpSpd,DTime1,pump1,valve1,AirDelay);  //Run Cleaning w/ pump1
         }
         if (currentDilutionTime1 - previousDilutionTime1 >= CTime1*60000  && Cleaning == 2 && tube1 == 1) 
         {
               Dilution(PumpSpd,DTime1,pump2,valve1,AirDelay);  //Run Cleaning w/ pump2
         }         
        
           // If time to Dilute Tube2
         if(currentDilutionTime2 - previousDilutionTime2 >= CTime2*60000 && Cleaning == 0 && tube2 == 1 && OD2 >= minthrs2) 
         {
               
              previousDilutionTime2 = currentDilutionTime2;   // save Dilution Time 
              Serial.println("AutDil");
              if (OD2 >= thrs2 && OD2 >= previousOD2)
              { 
                  Dilution(PumpSpd,DTime2,pump2,valve2,AirDelay);  //Run dilution
                  Tube2Pump = 2;
                  PumpUsage.Begin();
                  Serial.print("Tube2 - Pump2");
                  PumpUsage.End();
              }
              else 
              {
                  Dilution(PumpSpd,DTime2,pump1,valve2,AirDelay);  //Run dilution
                  Tube2Pump = 1;
                  PumpUsage.Begin();
                  Serial.print("Tube2 - Pump1");
                  PumpUsage.End();
              }
              previousOD2 = OD2;
         }  

         if (currentDilutionTime2 - previousDilutionTime2 >= CTime2*60000  && Cleaning == 1 && tube2 == 1) 
         {
               Dilution(PumpSpd,DTime2,pump1,valve2,AirDelay);  //Run Cleaning w/ pump1
         }
         if (currentDilutionTime2 - previousDilutionTime2 >= CTime2*60000  && Cleaning == 2 && tube2 == 1) 
         {
               Dilution(PumpSpd,DTime2,pump2,valve2,AirDelay);  //Run Cleaning w/ pump2
         }       

         
          // If time to Dilute Tube3
         if(currentDilutionTime3 - previousDilutionTime3 >= CTime3*60000  && Cleaning == 0 && tube3 == 1 && OD3 >= minthrs3) 
         {
               
              previousDilutionTime3 = currentDilutionTime3;   // save Dilution Time 
              Serial.println("AutDil");
              if (OD3 >= thrs3 && OD3 >= previousOD3)
              { 
                  Dilution(PumpSpd,DTime3,pump2,valve3,AirDelay);  //Run dilution
                  Tube3Pump = 2;
                  PumpUsage.Begin();
                  Serial.print("Tube3 - Pump2");
                  PumpUsage.End();
              }
              else 
              {
                  Dilution(PumpSpd,DTime3,pump1,valve3,AirDelay);  //Run dilution
                  Tube3Pump = 1;
                  PumpUsage.Begin();
                  Serial.print("Tube3 - Pump1");
                  PumpUsage.End();
              }
              previousOD3 = OD3;
         }  

          
         if (currentDilutionTime3 - previousDilutionTime3 >= CTime3*60000  && Cleaning == 1 && tube3 == 1) 
         {
               Dilution(PumpSpd,DTime3,pump1,valve3,AirDelay);  //Run Cleaning w/ pump1
         }
         if (currentDilutionTime3 - previousDilutionTime3 >= CTime1*60000  && Cleaning == 2 && tube3 == 1) 
         {
               Dilution(PumpSpd,DTime3,pump2,valve3,AirDelay);  //Run Cleaning w/ pump2
         }            
       
          
          // If time to Dilute Tube4  
         if(currentDilutionTime4 - previousDilutionTime4 >= CTime4*60000  && Cleaning == 0 && tube4 == 1 && OD4 >= minthrs4) 
         {
               
              previousDilutionTime4 = currentDilutionTime4;   // save Dilution Time 
              Serial.println("AutDil");
              if (OD4 >= thrs4 && OD4 >= previousOD4)
              { 
                  Dilution(PumpSpd,DTime4,pump2,valve4,AirDelay);  //Run dilution
                  Tube4Pump = 2;
                  PumpUsage.Begin();
                  Serial.print("Tube4 - Pump2");
                  PumpUsage.End();
              }
              else 
              {
                  Dilution(PumpSpd,DTime4,pump1,valve4,AirDelay);  //Run dilution
                  Tube4Pump = 1;
                  PumpUsage.Begin();
                  Serial.print("Tube4 - Pump1");
                  PumpUsage.End();
              }
              previousOD4 = OD4;
         }  

         if (currentDilutionTime4 - previousDilutionTime4 >= CTime4*60000  && Cleaning == 1 && tube4 == 1) 
         {
               Dilution(PumpSpd,DTime4,pump1,valve4,AirDelay);  //Run Cleaning w/ pump1
         }
         if (currentDilutionTime4 - previousDilutionTime4 >= CTime4*60000  && Cleaning == 2 && tube4 == 1) 
         {
               Dilution(PumpSpd,DTime4,pump2,valve4,AirDelay);  //Run Cleaning w/ pump2
         }  

         
          // If time to Dilute Tube5
         if(currentDilutionTime5 - previousDilutionTime5 >= CTime5*60000  && Cleaning == 0 && tube5 == 1 && OD5 >= minthrs5) 
         {
               
              previousDilutionTime5 = currentDilutionTime5;   // save Dilution Time 
              Serial.println("AutDil");
              if (OD5 >= thrs5 && OD5 >= previousOD5)
              { 
                  Dilution(PumpSpd,DTime5,pump2,valve5,AirDelay);  //Run dilution
                  Tube5Pump = 2;
                  PumpUsage.Begin();
                  Serial.print("Tube5 - Pump2");
                  PumpUsage.End();
              }
              else 
              {
                  Dilution(PumpSpd,DTime5,pump1,valve5,AirDelay);  //Run dilution
                  Tube5Pump = 1;
                  PumpUsage.Begin();
                  Serial.print("Tube5 - Pump1");
                  PumpUsage.End();
              }
              previousOD5 = OD5;
         } 

         if (currentDilutionTime5 - previousDilutionTime5 >= CTime5*60000  && Cleaning == 1 && tube5 == 1) 
         {
               Dilution(PumpSpd,DTime5,pump1,valve5,AirDelay);  //Run Cleaning w/ pump1
         }
         if (currentDilutionTime5 - previousDilutionTime5 >= CTime5*60000  && Cleaning == 2 && tube5 == 1) 
         {
               Dilution(PumpSpd,DTime5,pump2,valve5,AirDelay);  //Run Cleaning w/ pump2
         }           
          // If time to Dilute Tube6
         if(currentDilutionTime6 - previousDilutionTime6 >= CTime6*60000  && Cleaning == 0 && tube6 == 1 && OD6 >= minthrs6) 
         {
               
              previousDilutionTime6 = currentDilutionTime6;   // save Dilution Time 
              Serial.println("AutDil");
              if (OD6 >= thrs6 && OD6 >= previousOD6)
              { 
                  Dilution(PumpSpd,DTime6,pump2,valve6,AirDelay);  //Run dilution
                  Tube6Pump = 2;
                  PumpUsage.Begin();
                  Serial.print("Tube6 - Pump2");
                  PumpUsage.End();
              }
              else 
              {
                  Dilution(PumpSpd,DTime6,pump1,valve6,AirDelay);  //Run dilution
                  Tube6Pump = 1;
                  PumpUsage.Begin();
                  Serial.print("Tube6 - Pump1");
                  PumpUsage.End();
              }
              previousOD6 = OD6;
         } 
         if (currentDilutionTime6 - previousDilutionTime6 >= CTime6*60000  && Cleaning == 1 && tube6 == 1) 
         {
               Dilution(PumpSpd,DTime6,pump1,valve6,AirDelay);  //Run Cleaning w/ pump1
         }
         if (currentDilutionTime6 - previousDilutionTime6 >= CTime6*60000  && Cleaning == 2 && tube6 == 1) 
         {
               Dilution(PumpSpd,DTime6,pump2,valve6,AirDelay);  //Run Cleaning w/ pump2
         }  
      }// Is automatic Dilution On
    } // Is reading on
  }   // Time to read          
}     // void loop



