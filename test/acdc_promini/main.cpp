#include <Arduino.h>
#include <mySensors.h>
#include <utilities.h>

unsigned long previousMillis = 0;

void setup() {
    Serial.begin(115200);    
    initSensors();
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= SEND_INTERVAL) {
        sendDataPZEM();
        sendDataADS();
        
        previousMillis = currentMillis;
    }
}
