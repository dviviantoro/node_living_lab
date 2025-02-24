#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
// #include <mySensors.h>
#include <utilities.h>
#include <esp_wifi.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

const char* ssids[] = {"hottuf", "hotgalaxy", "raspberry123"};
const char* passwords[] = {"bismillah", "bismillah", "nasigodhog1"};
const int numNetworks = 3;

RTC_DATA_ATTR int fail_sendDataCount = 0;
// MAC: dc:54:75:f1:7b:9c
uint8_t broadcastAddress[] = {0xDC, 0x54, 0x75, 0xF1, 0x7B, 0x9C};

unsigned long previousMillisSendData = 0;
unsigned long previousMillisCheckWifi = 0;
const long intervalSendData = 3000;
const long intervalCheckWifi = 10000;

bool wifiConnected = false;

AsyncWebServer server(80);
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

void initElegantOTA() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Hi! I am ESP32.");
    });

    server.begin();
    Serial.println("HTTP server started");

    ElegantOTA.begin(&server);    // Start ElegantOTA
}

void connectToWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    int currentNetwork = 0;
  
    // Loop through each SSID until connected
    while (currentNetwork < numNetworks && !wifiConnected) {
        Serial.print("Trying to connect to ");
        Serial.println(ssids[currentNetwork]);
    
        WiFi.begin(ssids[currentNetwork], passwords[currentNetwork]); // Try connecting to the current SSID
    
        unsigned long startAttemptTime = millis();
    
        // Attempt to connect to the Wi-Fi network
        while (WiFi.status() != WL_CONNECTED) {
            if (millis() - startAttemptTime >= 5000) { // 10 seconds timeout
                Serial.print("Failed to connect to ");
                Serial.println(ssids[currentNetwork]);
                break; // Move to the next network
            }
            delay(500);
            Serial.print(".");
        }
    
        // Check if connected
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("");
            Serial.println("Connected to WiFi!");
            Serial.print("IP Address: ");
            Serial.println(WiFi.localIP());
            wifiConnected = true; // Set connected to true, stop searching for networks

            initElegantOTA();
        } else {
            currentNetwork++; // Move to the next network in the list
        }
    }
  
    if (!wifiConnected) {
      Serial.println("Unable to connect to any of the available WiFi networks.");
    }
}

void resetWifiCahnnel() {
    if (!wifiConnected) {
        WiFi.disconnect(true);
        WiFi.mode(WIFI_STA);
        esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    }
}

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    resetWifiCahnnel();
    initESPNOW();
}

int counter = 0;
void loop() {   
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillisSendData >= intervalSendData) {
        previousMillisSendData = currentMillis;

        counter ++;
        if (counter > 10) {
            ESP.restart();
        }
        
        uint8_t wifi_channel = WiFi.channel();
        Serial.print("Transmitter Wi-Fi Channel: ");
        Serial.println(wifi_channel);

        String payload = DEVICE + String(",") + "test123,567,890,";
        Serial.println(payload);
        sendStringData(payload);
    }

    if (wifiConnected) {
        // Serial.println("masuk ota");
        ElegantOTA.loop();
    }
}