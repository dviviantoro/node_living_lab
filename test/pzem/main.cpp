#include <PZEM004Tv30.h>
#include <Wire.h>

// 1 fail
// PZEM004Tv30 pzem(5, 4);
// 2 fail
// PZEM004Tv30 pzem(7, 6);
// 3 pass
// PZEM004Tv30 pzem(8, 9);
// 4 fail
// PZEM004Tv30 pzem(10,16);
// 5 pass
// PZEM004Tv30 pzem(15,14);
// 6  fail
// PZEM004Tv30 pzem(18,19);

// pair 1
// PZEM004Tv30 pzem(8, 4); // ch3 pcb1
// PZEM004Tv30 pzem(9, 5); // ch4 pcb3

// pair 2
// PZEM004Tv30 pzem(14, 6); // ch1 pcb2
// PZEM004Tv30 pzem(15, 7); // ch5 pcb5

// pair 3
// PZEM004Tv30 pzem(10, 18); // ch2 pcb2
// PZEM004Tv30 pzem(16, 19); // ch6 pcb5


void setup() {
  Serial.begin(9600);
}

void loop() {
  float voltage = pzem.voltage();
  if (voltage != NAN) {
    Serial.print("Voltage: ");
    Serial.print(voltage);
    Serial.println("V");
  } else {
    Serial.println("Error reading voltage");
  }

  float current = pzem.current();
  if (current != NAN) {
    Serial.print("Current: ");
    Serial.print(current);
    Serial.println("A");
  } else {
    Serial.println("Error reading current");
  }

  float power = pzem.power();
  if (current != NAN) {
    Serial.print("Power: ");
    Serial.print(power);
    Serial.println("W");
  } else {
    Serial.println("Error reading power");
  }

  float energy = pzem.energy();
  if (current != NAN) {
    Serial.print("Energy: ");
    Serial.print(energy, 3);
    Serial.println("kWh");

  } else {
    Serial.println("Error reading energy");
  }

  float frequency = pzem.frequency();
  if (current != NAN) {
    Serial.print("Frequency: ");
    Serial.print(frequency, 1);
    Serial.println("Hz");
  } else {
    Serial.println("Error reading frequency");
  }

  float pf = pzem.pf();
  if (current != NAN) {
    Serial.print("PF: ");
    Serial.println(pf);
  } else {
    Serial.println("Error reading power factor");
  }

  Serial.println();
  delay(2000);
}