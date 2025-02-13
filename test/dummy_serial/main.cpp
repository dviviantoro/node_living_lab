#include <Arduino.h>

String irrDevc[3] = {"IRR-1", "IRR-2", "IRR-3"};
String ambDevc[4] = {"AMB-1", "AMB-2", "AMB-3", "AMB-4"};
String surDevc[2] = {"SUR-1", "SUR-2"};
String acChannel[6] = {"AC-1", "AC-2", "AC-3", "AC-4", "AC-5", "AC-6"};
String dcChannel[4] = {"DC-1", "DC-2", "DC-3", "DC-4"};

void dummyIrr() {
    int length = sizeof(irrDevc) / sizeof(irrDevc[0]);
    // data = device_id,irradiance, temp, hum
    for (int i = 0; i < length; i ++) {
        String data = irrDevc[i] + ",";
        data += String(random(100, 10000)) + ",";
        data += String(random(25, 35)) + ",";
        data += String(random(50, 100));
        
        Serial.println(data);
        delay(1000);
    }
}

void dummyAmb() {
    int length = sizeof(ambDevc) / sizeof(ambDevc[0]);
    // data = device_id,vbat,temp,hum,lux,rain
    for (int i = 0; i < length; i ++) {
        String data = ambDevc[i] + ",";
        data += String(random(3500, 4200)) + ",";
        data += String((float)random(25, 35)) + ",";
        data += String(random(50, 100)) + ",";
        data += String(random(1000, 10000)) + ",";
        data += String(random(50, 1000));
        
        Serial.println(data);
        delay(1000);
    }
}

void dummySur() {
    int length = sizeof(surDevc) / sizeof(surDevc[0]);
    // data = device_id,vbat,temp_top,temp_bot
    for (int i = 0; i < length; i ++) {
        String data = surDevc[i] + ",";
        data += String(random(3500, 4200)) + ",";
        data += String((float)random(25, 35)) + ",";
        data += String((float)random(25, 35));
        
        Serial.println(data);
        delay(1000);
    }
}

void dummyAC() {
    int length = sizeof(acChannel) / sizeof(acChannel[0]);
    // data = device_id,volt,current,power,energy,freq,pf
    for (int i = 0; i < length; i ++) {
        String data = acChannel[i] + ",";
        data += String((float)random(200, 240)) + ",";
        data += String((float)random(1, 10)) + ",";
        data += String((float)random(500, 10000)) + ",";
        data += String((float)random(10000, 100000)) + ",";
        data += String((float)random(48, 52)) + ",";
        data += String((float)random(0, 5));
        
        Serial.println(data);
        delay(1000);
    }
}

void dummyDC() {
    int length = sizeof(dcChannel) / sizeof(dcChannel[0]);
    // data = device_id,volt,current
    for (int i = 0; i < length; i ++) {
        String data = dcChannel[i] + ",";
        data += String((float)random(44, 50)) + ",";
        data += String((float)random(1, 10));
        
        Serial.println(data);
        delay(1000);
    }
}

void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Dummy start!");
}

void loop() {
    dummyIrr();
    dummyAmb();
    dummySur();
    dummyAC();
    dummyDC();
}