#include <mySensors.h>
#include <utilities.h>

SHT2x sht;
BH1750 lightMeter;

void initSensors() {
    Wire.begin();
    sht.begin();
    lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
    
    pinMode(BAT_PIN, INPUT);
    pinMode(RAIN_SENSOR_PIN, INPUT);
    pinMode(SWITCH_ON_PIN, OUTPUT);
    digitalWrite(SWITCH_ON_PIN, HIGH);
}

// int rainfall() {}

float agregateBat() {
    uint32_t Vbatt = 0;
    for(int i = 0; i < VBAT_SAMPLING; i++) {
        Vbatt = Vbatt + analogReadMilliVolts(A0);
    }
    float Vbattf = 2 * Vbatt / VBAT_SAMPLING / 1000.0;
    return Vbattf;
}

String compileData() {
    sht.read();

    float vBat = agregateBat();
    float temp = sht.getTemperature();
    int hum = sht.getHumidity();
    int lux = lightMeter.readLightLevel();
    lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
    int rain = digitalRead(RAIN_SENSOR_PIN);

    String myData = String(vBat) + ",";
    myData += String(temp) + ",";
    myData += String(hum) + ",";
    myData += String(lux) + ",";
    myData += String(rain);

    return myData;
}