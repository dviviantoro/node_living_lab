#include <mySensors.h>
#include <utilities.h>

SoftwareSerial espSerial(ESP01_RX, ESP01_TX);

ADS1115 ads1(0x48);
ADS1115 ads2(0x49);

const int PZEM_RX_PINS[NUM_SENSORS] = {14, 10, 8, 9, 15, 16};
const int PZEM_TX_PINS[NUM_SENSORS] = {6, 18, 4, 5, 7, 19};

String ac_channels[6] = {AC_CH_1, AC_CH_2, AC_CH_3, AC_CH_4, AC_CH_5, AC_CH_6};

String dc_channels_name[4] = {DC_CH_1, DC_CH_2, DC_CH_3, DC_CH_4};
int dc_channels_cur[4] = {3, 0, 2, 1};
int dc_channels_vol[4] = {0, 3, 1, 2};

SoftwareSerial pzemSerial[NUM_SENSORS] = {
    SoftwareSerial(PZEM_RX_PINS[0], PZEM_TX_PINS[0]),
    SoftwareSerial(PZEM_RX_PINS[1], PZEM_TX_PINS[1]),
    SoftwareSerial(PZEM_RX_PINS[2], PZEM_TX_PINS[2]),
    SoftwareSerial(PZEM_RX_PINS[3], PZEM_TX_PINS[3]),
    SoftwareSerial(PZEM_RX_PINS[4], PZEM_TX_PINS[4]),
    SoftwareSerial(PZEM_RX_PINS[5], PZEM_TX_PINS[5])
};

PZEM004Tv30 pzem[NUM_SENSORS] = {
    PZEM004Tv30(pzemSerial[0]),
    PZEM004Tv30(pzemSerial[1]),
    PZEM004Tv30(pzemSerial[2]),
    PZEM004Tv30(pzemSerial[3]),
    PZEM004Tv30(pzemSerial[4]),
    PZEM004Tv30(pzemSerial[5])
};

void initSensors() {
    espSerial.begin(115200);

    Wire.begin();
    ads1.begin();
    ads1.setGain(1);
    ads1.setDataRate(2);
    ads2.begin();
    ads2.setGain(1);
    ads2.setDataRate(2);
    
    for (int i = 0; i < NUM_SENSORS; i++) {
        pzemSerial[i].begin(9600); 
    }
}

String compileDataADS(int channel) {
    float adcVT = 0;
    float adcCT = 0;
    float adcVTSum = 0;
    float adcCTSum = 0;
    float current = 0;
    float voltage = 0;
    
    for (int i = 0; i < NUM_SAMPLES; i ++) {
        adcVTSum += ads1.readADC(dc_channels_vol[channel]);
        adcCTSum += ads2.readADC(dc_channels_cur[channel]);
    }
    
    adcVT = adcVTSum / NUM_SAMPLES;
    adcCT = adcCTSum / NUM_SAMPLES;
    // String adsData = String(adcVT) + "," + String(adcCT);
    
    voltage = adcVT;
    current = 0.0111 * adcCT - 214;
    String adsData = String(voltage) + "," + String(current);
    
    return adsData;
}

String compileDataPZEM(PZEM004Tv30 pzemN) {
    float voltage = 0;
    float current = 0;
    float power = 0;
    float energy = 0;
    float frequency = 0;
    float pf = 0;
    
    float voltageSum = 0;
    float currentSum = 0;
    float powerSum = 0;
    float energySum = 0;
    float frequencySum = 0;
    float pfSum = 0;

    float voltageAvg = 0;
    float currentAvg = 0;
    float powerAvg = 0;
    float energyAvg = 0;
    float frequencyAvg = 0;
    float pfAvg = 0;

    int voltageReadCount = 0;
    int currentReadCount = 0;
    int powerReadCount = 0;
    int energyReadCount = 0;
    int frequencyReadCount = 0;
    int pfReadCount = 0;

    for (int h = 0; h < NUM_SAMPLES; h ++){
        voltage = pzemN.voltage();
        current = pzemN.current();
        power = pzemN.power();
        energy = pzemN.energy();
        frequency = pzemN.frequency();
        pf = pzemN.pf();

        // voltage = random(220, 240);
        // current = random(6, 10);
        // power = random(11, 20);
        // energy = random(21, 30);
        // frequency = random(48, 53);
        // pf = random(1, 3);

        if (!isnan(voltage)) {voltageSum += voltage; voltageReadCount ++;};
        if (!isnan(current)) {currentSum += current; currentReadCount ++;};
        if (!isnan(power)) {powerSum += power; powerReadCount ++;};
        if (!isnan(energy)) {energySum += energy; energyReadCount ++;};
        if (!isnan(frequency)) {frequencySum += frequency; frequencyReadCount ++;};
        if (!isnan(pf)) {pfSum += pf; pfReadCount ++;};
        delay(10);
    }

    voltageAvg = voltageSum / voltageReadCount;
    currentAvg = currentSum / currentReadCount;
    powerAvg = powerSum / powerReadCount;
    energyAvg = energySum / energyReadCount;
    frequencyAvg = frequencySum / frequencyReadCount;
    pfAvg = pfSum / pfReadCount;
    
    String pzemData = String(voltageAvg, 2) + ",";
    pzemData += String(currentAvg, 2) + ",";
    pzemData += String(powerAvg, 2) + ",";
    pzemData += String(energyAvg, 2) + ",";
    pzemData += String(frequencyAvg, 2) + ",";
    pzemData += String(pfAvg, 2);

    return pzemData;
}

void sendDataPZEM() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        String payload = ac_channels[i] + "," + compileDataPZEM(pzem[i]);
        espSerial.println(payload);
        Serial.println(payload);
        delay(300);
    }
}

void sendDataADS() {
    for (int i = 0; i < NUM_DC_CHANNELS; i++) {
        String payload = dc_channels_name[i] + "," + compileDataADS(i);
        espSerial.println(payload);
        Serial.println(payload);
        delay(300);
    }
}