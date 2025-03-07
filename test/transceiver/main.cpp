#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// if you need to parse the incoming data
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
}

void sendStringData(String msg) {
    uint8_t *buffer = (uint8_t*) msg.c_str();
    size_t sizeBuff = sizeof(buffer) * msg.length();
    esp_now_send(broadcastAddress, buffer, sizeBuff);
}

void setup() {
    Serial.begin(115200);
    
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    
    // esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
    esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
    esp_now_register_recv_cb(OnDataRecv);
}

// bool switchFlag = false;
// int switchInterval = 0;

void loop() {
    if (Serial.available() > 0) {
        String payload = Serial.readString();
        sendStringData(payload);

        // if (getValue(incomingSerial, ',', 0) == "SW") {
        //     switchFlag = true;
        //     switchInterval = getValue(incomingSerial, ',', 1).toInt();
        // }
    }
}
