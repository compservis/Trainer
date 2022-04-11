#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include "GyverTM1637.h"


#define SPEED_MIN 200
#define SPEED_MAX 1500

#define TIME_MIN 1
#define TIME_MAX 30

#define HITSENSOR 2 
#define stop_pin 3 

#define r1_pin 23 
#define r2_pin 25 
#define r3_pin 27 
#define r4_pin 29 
#define r5_pin 31 
#define biper_pin 4
//Pots
#define timepot_pin A15 
#define speedpot_pin A14 
//Relays
#define relay1_pin 12 
#define relay2_pin 2 
#define relay3_pin 3 

#define BUTTONS_PIN A0 

#define ledstart_pin 23
#define ledstop_pin 25
#define reg1_pin 27
#define reg2_pin 29
#define reg3_pin 31

//led panel
#define l1_pin 48
#define l2_pin 46
#define l3_pin 44
#define l4_pin 42
#define l5_pin 40
#define l6_pin 38
#define l7_pin 36
#define l8_pin 34
#define l9_pin 32
#define l10_pin 30

#define TIME_COEF 1

#define CLK 7 
#define DIO 8


#define KCAL_RATIO 0.158 


// ----

LiquidCrystal_I2C lcd(0x27, 20, 4);
GyverTM1637 disp(CLK, DIO);

// ----

int ledPins[] = {
  48, 46, 44, 42, 40, 38, 36, 34, 32, 30 
};

int indPins[] = {r1_pin, r2_pin, r3_pin, r4_pin, r5_pin}; 

int pinCount = 10;
int Z = 0;

unsigned int timew = 0; 
unsigned long speedw = 0; 
unsigned int timewold = 0;
unsigned int speedwold = 0;

int tmin = 0; // timer
int tsec = 0;

int mode;

unsigned long timeMark;
unsigned long prevTimeMark;
unsigned long prevTimeMarkPunch;
unsigned long prevTimeMarkButtonLag;
unsigned long prevPunchTime;

boolean isTraining;
boolean preliminaryLastFinish;
unsigned int lastPunchNumber;
int ledCount;
int sensorReading;
int ledLevel;
int thisLed;

int buttonPress;


const byte maxDataLength = 20; 
char receivedChars[maxDataLength+1] ;
boolean newData = false;

// ---

unsigned long lasthit = 0;
unsigned int hitcount = 0;

bool hitSensorData;



int hitAverage = 0;
int hitBounds = 800;

const int historyElements = 10;

int bpmHistory[historyElements];
int hitHistory[historyElements];
bool decreasePunchSpeedNeeded = false;
bool encreasePunchSpeedNeeded = false;
bool adjustFinished = false;
// bool b;
const int speedLLimit = 2500;
const int speedHLimit = 500;

unsigned long decTime = 0;
unsigned long startTime = 0;

int Threshold = 550;

// ----

void updateDisplay(); 
void training();
void tick();

void punch(int i); 
void nextPunch();

void testLEDs();
void setLedLevel(int i);
void setIndicator(int i);
