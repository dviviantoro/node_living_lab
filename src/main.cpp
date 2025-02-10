#include <Arduino.h>
#define SWITCH  5

void setup() {
    Serial.begin(9600);
    pinMode(SWITCH, OUTPUT);
}

void loop() {
    digitalWrite(SWITCH, HIGH);
    delay(3000);
    digitalWrite(SWITCH, LOW);
    delay(3000);
}