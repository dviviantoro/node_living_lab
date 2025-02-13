#include <mySensors.h>
#include <utilities.h>

SHT2x sht;
BH1750 lightMeter;

volatile long int jumlah_tip = 0;
long int temp_jumlah_tip = 0;
float curah_hujan = 0.00;
const float milimeter_per_tip = 0.70;
volatile boolean flag = false;

void ICACHE_RAM_ATTR hitung_curah_hujan();

void initSensors() {
    Wire.begin();
    sht.begin();
    lightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);
    
    pinMode(BAT_PIN, INPUT);
    pinMode(RAIN_SENSOR_PIN, INPUT);
    pinMode(SWITCH_ON_PIN, OUTPUT);
    digitalWrite(SWITCH_ON_PIN, HIGH);
    pinMode(RAIN_SENSOR_PIN, INPUT);

    attachInterrupt(digitalPinToInterrupt(RAIN_SENSOR_PIN), hitung_curah_hujan, FALLING);
}

void ICACHE_RAM_ATTR hitung_curah_hujan() {
    flag = true;
}

void updateRainfall() {
    if (flag) {
        jumlah_tip++;
        curah_hujan = jumlah_tip * milimeter_per_tip;
        flag = false;
    }
}

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
    updateRainfall();

    float vBat = agregateBat();
    float temp = sht.getTemperature();
    int hum = sht.getHumidity();
    int lux = lightMeter.readLightLevel();
    lightMeter.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
    int rain = curah_hujan;

    String myData = String(vBat) + ",";
    myData += String(temp) + ",";
    myData += String(hum) + ",";
    myData += String(lux) + ",";
    myData += String(rain);

    return myData;
}