#include <Wire.h>
#include <INA226.h>
#include <Adafruit_AHTX0.h>

void initSensors();
double agregateData();

extern class Adafruit_AHTX0 aht;