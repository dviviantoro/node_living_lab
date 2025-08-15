#include <Arduino.h>
#include <ambSensor.h>
#include <utilities.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

#define DEVICE_ID       "AMB-2"
#define DEVICE_LAYER    1
#define BUTTON_PIN      3

const uint8_t* all_addresses[] = {
    // dongle,
    // ambient_1,
    // ambient_2,
    bridge_1,
    // irradiance_1,
    // surface_1,
    // irradiance_2,
    // surface_2,
    // irradiance_3
};
const int num_addresses = sizeof(all_addresses) / sizeof(all_addresses[0]);

const char* ssids[] = {"hotgalaxy", "raspberry123"};
const char* passwords[] = {"bismillah", "nasigodhog1"};
const int numNetworks = 3;
bool wifiConnected = false;
AsyncWebServer server(80);
esp_now_peer_info_t peerInfo;

unsigned long previousMillisSendData = 0;
int sendInterval;
bool lastSendSuccess = true;

void initElegantOTA() {
    String message = "ElegantOTA is running. \nDevice_ID is: " + String(DEVICE_ID);

    server.on("/", HTTP_GET, [message](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", message);
    });
    server.begin();
    Serial.println("HTTP server started");
    ElegantOTA.begin(&server);    // Start ElegantOTA
}

void sendStringData(String msg, const uint8_t *mac) {
    Serial.print("Send msg: ");
    Serial.println(*mac);

    uint8_t *buffer = (uint8_t*) msg.c_str();
    size_t sizeBuff = sizeof(buffer) * msg.length();
    esp_err_t result = esp_now_send(mac, buffer, sizeBuff);

    if (result == ESP_OK) {
        Serial.println("msg sent successfully.");
    } else {
        Serial.println("Failed to send msg.");
    }
}

void connectToWiFi() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    int currentNetwork = 0;
  
    while (currentNetwork < numNetworks && !wifiConnected) {
        Serial.print("Trying to connect to ");
        Serial.println(ssids[currentNetwork]);
    
        WiFi.begin(ssids[currentNetwork], passwords[currentNetwork]); // Try connecting to the current SSID
    
        unsigned long startAttemptTime = millis();
    
        while (WiFi.status() != WL_CONNECTED) {
            if (millis() - startAttemptTime >= 5000) { // 10 seconds timeout
                Serial.print("Failed to connect to ");
                Serial.println(ssids[currentNetwork]);
                break; // Move to the next network
            }
            delay(500);
            Serial.print(".");
        }
    
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

void retrySendPayload(String payload) {
    for (int i = 0; i < num_addresses; i++) {
        const uint8_t* current_address = all_addresses[i];

        for (int attempt = 1; attempt <= 5; attempt++) {
            sendStringData(payload, current_address);
            delay(50);
            if (lastSendSuccess) {
                Serial.print("  Attempt ");
                Serial.print(attempt);
                Serial.println(": ✅ Success!");
                break;
            } else {
                Serial.print("  Attempt ");
                Serial.print(attempt);
                Serial.println(": ❌ Failed.");
            }                
        }
    }
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "[ESP-NOW] delivery success" : "[ESP-NOW] delivery fail");
    lastSendSuccess = (status == ESP_NOW_SEND_SUCCESS);
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    char* buff = (char*) incomingData;
    String buffStr = String(buff);
    Serial.println(buffStr);
    retrySendPayload(buffStr);
}

void setupPeer(const uint8_t *mac) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (!esp_now_is_peer_exist(mac)) {
        if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    connectToWiFi();
    if (!wifiConnected) {
        resetWifiCahnnel();
    }
    
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_register_send_cb(OnDataSent);
    
    for (int i = 0; i < num_addresses; i++) {
        const uint8_t* current_address = all_addresses[i];
        setupPeer(current_address);
        delay(100);
    }
    
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

    initSensors();
    sendInterval = random(1000, 10000);
}

void loop() {
    unsigned long currentMillis = millis();
    if (digitalRead(BUTTON_PIN)) ESP.restart();
    if (wifiConnected) ElegantOTA.loop();
    
    if (currentMillis - previousMillisSendData >= sendInterval) {
        sendInterval = random(50000, 60000);
        previousMillisSendData = currentMillis;

        String payload = String(DEVICE_LAYER) + String(",");
        payload += String(DEVICE_ID) + String(",");
        payload += compileData();
        Serial.println(payload);
        
        retrySendPayload(payload);
    }
}