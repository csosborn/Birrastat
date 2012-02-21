#include <SafetyPin.h>
#include <Thermistor.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <PID_v1.h>

#include "Bicolor.h"
#include "SetpointKnob.h"

typedef struct {
  temperature_t T;
  float V;
} cal_sample_t;

typedef struct {
  cal_sample_t S1;
  cal_sample_t S2;
  cal_sample_t S3;
} calibration_set_t;

calibration_set_t calSet;

//////////////////////////////////////////////
/**
 * Hardware Initialization
 */

// Teensy 2.0 Pinout
namespace pin {
  DigitalOut lcdRegSel(0);
  DigitalOut lcdEnable(1);
  // 2 
  DigitalOut lcdData5(3);
  DigitalOut lcdData6(4);
  DigitalOut lcdData7(5);
  // 6
  // 7
  // 8
  // 9
  // 12
  DigitalOut onboardLed(11);    // Teensy's onboard LED
  DigitalOut lcdData4(12);
  AnalogIn tempProbe(13);
  AnalogIn tempSelect(14);
  DigitalOut coolerDrive(15);
  DigitalOut heatsinkFan(16);
  // 17
  // 18
  // 19
  DigitalOut bicolorRed(20);
  DigitalOut bicolorGreen(21);
};

// Instantiate hardware components

// the LCD character display
LiquidCrystal lcd(pin::lcdRegSel, pin::lcdEnable, pin::lcdData4, pin::lcdData5, pin::lcdData6, pin::lcdData7);

// the red/green bicolor LED
Bicolor bcl(pin::bicolorGreen, pin::bicolorRed);

// the calibrated thermistor
Thermistor thermistor(pin::tempProbe, 14780); 

SetpointKnob spk(pin::tempSelect, 48, 75);

const byte MAX_ERROR = 1;

// main temperature control loop variables
double currentTemp = 0;    // the current measured temperature
double setpointTemp = 0;   // the setpoint temperature
double coolerPower = 0;     // the current cooler power level, ranging from 0 to 255

const int kSamplePeriod = 1000;

// PID controller for the main temperature control loop
PID thermostatPID(&currentTemp, &coolerPower, &setpointTemp, 50, 10, 0, REVERSE);

// cooler drive PWM loop counter
byte coolerPWMCounter = 0;

// The heatsink fan turns on when the cooler does and stays on for a period after the 
// cooler turns off. 
unsigned long lastCoolerTime = -1;  // the last time the cooler was on, in millis
const unsigned long fanLingerTime = 60000;    // the fan runs until the cooler has been off for at least a minute

unsigned long lastSampleTime = 0;

void updateSetTemp(temperature_t newTemp) {
  if (setpointTemp != newTemp) {
    setpointTemp = newTemp;
    lcd.setCursor(11, 0);
    lcd.print((int)setpointTemp);
    lcd.print((char)223);
    lcd.print("  ");
  }
}

void updateCurrTemp(temperature_t newTemp) {
  currentTemp = newTemp;
  lcd.setCursor(11, 1);
  lcd.print(currentTemp, 1);
  lcd.print((char)223);
  lcd.print("  ");
}

void updatePower() {
//  pin::coolerDrive.write(coolerPower);
  lcd.setCursor(11, 3);
  lcd.print(map(coolerPower, 0, 255, 0, 100));
  lcd.print("%  ");
}

void toggleFan(boolean on) {
  if (on && !pin::heatsinkFan.read()) {
    Serial.println("turning on fan");
  } else if (!on && pin::heatsinkFan.read()) {
    Serial.println("turning off fan"); 
  }
  pin::heatsinkFan.write(on);
}

void setup() {

  Serial.begin(57600);
  
  pin::coolerDrive.low();
  pin::heatsinkFan.low(); 
  
  lcd.begin(4, 20);
  lcd.clear();
    
  calSet.S1.T = 72.5;
  calSet.S1.V = 585.4;
  calSet.S2.T = 103;
  calSet.S2.V = 742.5;
  calSet.S3.T = 36;
  calSet.S3.V = 359;
  
  thermistor.calibrate(calSet.S1.T, calSet.S1.V, calSet.S2.T, calSet.S2.V, calSet.S3.T, calSet.S3.V);
    
  lcd.setCursor(0, 0);
  lcd.print("Set temp: ");
  
  lcd.setCursor(0, 1);
  lcd.print("Curr temp: ");
  
  lcd.setCursor(0, 3);
  lcd.print("Power: ");
  
  thermostatPID.SetSampleTime(kSamplePeriod);
  
  //turn the PID on
  thermostatPID.SetMode(AUTOMATIC);
}

void loop() {

  unsigned long now = millis();
  
  updateSetTemp((float)spk.read());
  
  if (coolerPWMCounter++ == 0) {

    if (now > (lastSampleTime + kSamplePeriod)) {
      lastSampleTime = now;
    
      pin::onboardLed.low();
  
      delay(20);      
      updateCurrTemp(thermistor.temperature());
      pin::onboardLed.write(coolerPower > coolerPWMCounter);

      updatePower();
    
      if (coolerPower > 0) {
        lastCoolerTime = now;
        toggleFan(true);
      } 
      else if (now > (lastCoolerTime + fanLingerTime)) {
        toggleFan(false);
      }
        
//      Serial.print(setpointTemp,1);
//      Serial.print(' ');
//      Serial.print(currentTemp,1);
//      Serial.print(' ');
//      Serial.println(coolerPower);
    }

    thermostatPID.Compute();

    lcd.setCursor(0, 2);
    lcd.print(coolerPower, 10);
    lcd.print("   ");
  
    bool coolerOn = coolerPower > coolerPWMCounter;
    pin::coolerDrive.write(coolerOn);
    pin::onboardLed.write(coolerOn);
  
  
    if (currentTemp > setpointTemp + MAX_ERROR) {
      bcl.red();
    } else {
      bcl.green();
    }


  }
      
  
}

