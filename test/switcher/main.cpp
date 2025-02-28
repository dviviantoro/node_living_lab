#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#define SW_PIN 0
#define BTN_PIN 2

int buttonState;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

bool switchFlag = false;
unsigned long previousMillis = 0;
int switchInterval = 1000;

String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length()-1;

    for(int i=0; i<=maxIndex && found<=index; i++)
    {
        if(data.charAt(i)==separator || i==maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1]+1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    char* buff = (char*) incomingData;
    String buffStr = String(buff);
    Serial.println(buffStr);

    if (getValue(buffStr, ',', 0) == "SW") {
        switchFlag = true;
        switchInterval = getValue(buffStr, ',', 1).toInt();
        previousMillis = millis();
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    pinMode(BTN_PIN, INPUT);
    pinMode(SW_PIN, OUTPUT);

    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= switchInterval) switchFlag = false;
    switchFlag ? digitalWrite(SW_PIN, HIGH) : digitalWrite(SW_PIN, LOW);

    int reading = digitalRead(BTN_PIN);
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == HIGH) {
                switchFlag = true;
            }
        }
    }
    lastButtonState = reading;
}
