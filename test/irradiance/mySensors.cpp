#include <mySensors.h>
#include <utilities.h>

INA226 ina(Wire);
Adafruit_AHTX0 aht;

int readings[NUM_READING];  // the readings from the analog input
int readIndex = 0;          // the index of the current reading
int total = 0;              // the running total
int averageValue = 0;            // the average

void initSensors() {
    Wire.begin();
    if(! ina.begin()) {
        Serial.println("INA226 Error");
        while(1);
    }
    Serial.println("INA226 Initialized");
    
    if (! aht.begin()) {
        Serial.println("AHT10 Error");
        while (1);
    }
    Serial.println("AHT10 Initialized");
    
    ina.configure(INA226_AVERAGES_1, INA226_BUS_CONV_TIME_1100US, INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);
    ina.calibrate(0.01, 4);   // Calibrate INA226. Rshunt = 0.01 ohm, Max excepted current = 4A
    ina.enableOverPowerLimitAlert();  // Enable Power Over-Limit Alert
    ina.setPowerLimit(0.130);
    ina.setAlertLatch(true);

    for (int thisReading = 0; thisReading < NUM_READING; thisReading++) {
        readings[thisReading] = 0;
    }
}

double agregateData() {
    int busVoltage, busPower, shuntVoltage, shuntCurrent;
    shuntCurrent = ina.readShuntCurrent() * 1000000;
    
    total = total - readings[readIndex];
    readings[readIndex] = shuntCurrent;
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
    if (readIndex >= NUM_READING) {
        readIndex = 0;
    }
    averageValue = total / NUM_READING;
    double realValue = 0.00502*(pow(averageValue,0.925));
    
    return realValue;
}

