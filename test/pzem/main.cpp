#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

SoftwareSerial pzemSWSerial(9,8);
PZEM004Tv30 pzem(pzemSWSerial);

void setup() {
    Serial.begin(9600);
    // pzem = PZEM004Tv30(pzemSWSerial);
    // pzemSWSerial.begin(9600);
}

void loop() {
    static uint8_t addr = SET_ADDRESS;

    // Print out current custom address
    Serial.print("Previous address:   0x");
    Serial.println(pzem.readAddress(), HEX);

    // // Set the custom address
    // Serial.print("Setting address to: 0x");
    // Serial.println(addr, HEX);
    // if(!pzem.setAddress(addr))
    // {
    //   // Setting custom address failed. Probably no PZEM connected
    //   Serial.println("Error setting address.");
    // } else {
    //   // Print out the new custom address
    //   Serial.print("Current address:    0x");
    //   Serial.println(pzem.readAddress(), HEX);
    //   Serial.println();
    // }

    // // Increment the address every loop if desired
    // if(INCREMENT){
    //   addr++;
    //   if(addr >= PZEM_DEFAULT_ADDR)
    //     addr = 0x01;
    // }

    delay(5000);

}