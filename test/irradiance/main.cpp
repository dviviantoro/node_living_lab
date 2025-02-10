#include <Arduino.h>
#include <mySensors.h>
#include <utilities.h>
#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x10, 0x52, 0x1C, 0xF3, 0xE7, 0x3C};
unsigned long sendPreviousMillis = 0;
int sendInterval = 5000;

esp_now_peer_info_t peerInfo;
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void initESPNOW() {
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(OnDataSent);
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
}

void sendStringData(String msg) {
    uint8_t *buffer = (uint8_t*) msg.c_str();
    size_t sizeBuff = sizeof(buffer) * msg.length();
    esp_err_t result = esp_now_send(broadcastAddress, buffer, sizeBuff);

    if (result == ESP_OK) {
        Serial.println("msg sent successfully.");
    } else {
        Serial.println("Failed to send msg.");
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    initSensors();
    initESPNOW();
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - sendPreviousMillis >= sendInterval) {
        sendPreviousMillis = currentMillis;
        sendInterval = random(1000, 30000);
        
        sensors_event_t humidity, temp;
        aht.getEvent(&humidity, &temp);
        double irradiance = agregateData();

        String msg = DEVICE_ID + String(";");
        msg += String(irradiance) + ";";
        msg += String(temp.temperature) + ";";
        msg += String(humidity.relative_humidity);
        Serial.println(msg);
        sendStringData(msg);
    }
}