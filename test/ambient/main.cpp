#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <mySensors.h>
#include <utilities.h>

RTC_DATA_ATTR int fail_sendDataCount = 0;
uint8_t broadcastAddress[] = {0x10, 0x52, 0x1C, 0xF3, 0xE7, 0x3C};

esp_now_peer_info_t peerInfo;
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "[ESP-NOW] delivery success" : "[ESP-NOW] delivery fail");
    status == ESP_NOW_SEND_SUCCESS ? fail_sendDataCount = 0 : fail_sendDataCount++;
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
        Serial.println("[ESP-NOW] message sent successfully");
    } else {
        Serial.println("[ESP-NOW] failed to send message");
    }
}

void testFunc() {
    String payload = DEVICE + String(",") + compileData();
    Serial.println(payload);
    sendStringData(payload);
    Serial.println(fail_sendDataCount);
    
    if (fail_sendDataCount >= MAX_ATTEMP) {
        fail_sendDataCount = 0;
        Serial.println("harusnya sleep nih");
        esp_deep_sleep_enable_gpio_wakeup(BITMASK_PIN_3, ESP_GPIO_WAKEUP_GPIO_HIGH);
        esp_deep_sleep_start();
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    initESPNOW();
    initSensors();

    String payload = DEVICE + String(",") + compileData();
    Serial.println(payload);
    sendStringData(payload);
    delay(100);
    
    if (fail_sendDataCount >= MAX_ATTEMP) {
        fail_sendDataCount = 0;
        delay(100);
        esp_deep_sleep_enable_gpio_wakeup(BITMASK_PIN_3, ESP_GPIO_WAKEUP_GPIO_HIGH);
    } else {
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    }
    
    Serial.println(fail_sendDataCount);
    Serial.println("Going to sleep now");
    digitalWrite(SWITCH_ON_PIN, LOW);
    esp_deep_sleep_start();
}

void loop() {
    // delay(3000);
    // testFunc();
}