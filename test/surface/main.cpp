#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <utilities.h>

OneWire oneWire1(OW1_PIN);
OneWire oneWire2(OW2_PIN);
DallasTemperature dallas1(&oneWire1);
DallasTemperature dallas2(&oneWire2);

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

float dallasSample(DallasTemperature dallasN) {
    uint32_t agregateDallas = 0;
    for(int i = 0; i < VAL_SAMPLING; i++) {
        dallasN.requestTemperatures();
        if (dallasN.getTempCByIndex(0) < 80) {
            return dallasN.getTempCByIndex(0);
        }
    }
}

float agregateBat() {
    uint32_t Vbatt = 0;
    for(int i = 0; i < VAL_SAMPLING; i++) {
        Vbatt = Vbatt + analogReadMilliVolts(A0);
    }
    float Vbattf = 2 * Vbatt / VAL_SAMPLING / 1000.0;
    return Vbattf;
}

String compileData() {
    float vBat = agregateBat();
    float top_temp = dallasSample(dallas1);
    float bot_temp = dallasSample(dallas2);

    String myData = String(vBat) + ",";
    myData += String(top_temp) + ",";
    myData += String(bot_temp);

    return myData;
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

    pinMode(SWITCH_ON_PIN, OUTPUT);
    digitalWrite(SWITCH_ON_PIN, HIGH);
    dallas1.begin();
    dallas2.begin();

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