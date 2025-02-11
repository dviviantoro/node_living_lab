#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t broadcastAddress[] = {0x10, 0x52, 0x1C, 0xF3, 0xE7, 0x3C};
unsigned long previousMillis = 0;
const long interval = 1000;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.println(sendStatus == 0 ? "[ESP-NOW] delivery success" : "[ESP-NOW] delivery fail");
}

void sendStringData(String msg) {
    uint8_t *buffer = (uint8_t*) msg.c_str();
    size_t sizeBuff = sizeof(buffer) * msg.length();
    esp_now_send(broadcastAddress, buffer, sizeBuff);
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        Serial.println("still alive now");
        
    }
    
    if (Serial.available()) {
        String payload = Serial.readStringUntil('\n');
        Serial.println("serial got data");
        Serial.println(payload);
        sendStringData(payload);
    }
}