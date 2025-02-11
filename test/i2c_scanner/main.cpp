#include <Arduino.h>
#include <Wire.h>
 
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\nI2C Scanner");
}
 
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}


/*
#include "ADS1X15.h"

ADS1115 ads1;
ADS1115 ads2(0x49);


void setup() {
    Serial.begin(115200);
    Wire.begin();

    ads1.begin();
    ads1.setGain(1);
    ads1.setDataRate(2);
    
    ads2.begin();
    ads2.setGain(1);
    ads2.setDataRate(2);
}


void loop() 
{

  int16_t val_0 = ads1.readADC(0);  
  int16_t val_1 = ads1.readADC(1);  
  int16_t val_2 = ads1.readADC(2);  
  int16_t val_3 = ads1.readADC(3);  

  float f = ads1.toVoltage(1);  //  voltage factor

  Serial.print("\tAnalog0 - 1: "); Serial.print(val_0); Serial.print('\t'); Serial.println(val_0 * f, 3);
  Serial.print("\tAnalog1 - 1: "); Serial.print(val_1); Serial.print('\t'); Serial.println(val_1 * f, 3);
  Serial.print("\tAnalog2 - 1: "); Serial.print(val_2); Serial.print('\t'); Serial.println(val_2 * f, 3);
  Serial.print("\tAnalog3 - 1: "); Serial.print(val_3); Serial.print('\t'); Serial.println(val_3 * f, 3);
  Serial.println();

  int16_t val_00 = ads2.readADC(0);  
  int16_t val_11 = ads2.readADC(1);  
  int16_t val_22 = ads2.readADC(2);  
  int16_t val_33 = ads2.readADC(3);  

  float ff = ads2.toVoltage(1);  //  voltage factor

  Serial.print("\tAnalog0 - 2: "); Serial.print(val_00); Serial.print('\t'); Serial.println(val_00 * ff, 3);
  Serial.print("\tAnalog1 - 2: "); Serial.print(val_11); Serial.print('\t'); Serial.println(val_11 * ff, 3);
  Serial.print("\tAnalog2 - 2: "); Serial.print(val_22); Serial.print('\t'); Serial.println(val_22 * ff, 3);
  Serial.print("\tAnalog3 - 2: "); Serial.print(val_33); Serial.print('\t'); Serial.println(val_33 * ff, 3);
  Serial.println();

  delay(1000);
}
*/

