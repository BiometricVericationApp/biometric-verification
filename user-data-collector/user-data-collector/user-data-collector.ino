#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4; // Aumenta esto para más promedio. 4 está bien.
byte rates[RATE_SIZE]; // Array de ritmos cardíacos
byte rateSpot = 0;
long lastBeat = 0; // Tiempo en el que ocurrió el último latido

float beatsPerMinute;
int beatAvg;
int GSR = A0;
int sensorValue;

long lastPrintTime = 0; // Guardar la última vez que se imprimió la información
const long printInterval = 3000; // Intervalo entre impresiones (en milisegundos)

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) { // Usar puerto I2C por defecto, velocidad 400kHz
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); // Configurar sensor con ajustes predeterminados
  particleSensor.setPulseAmplitudeRed(0x0A); // Encender LED rojo a bajo para indicar que el sensor está funcionando
  particleSensor.setPulseAmplitudeGreen(0); // Apagar LED verde
}

void loop() {
  sensorValue = analogRead(GSR);
  float conductiveVoltage = sensorValue * (5.0 / 1023.0);
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true) {
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20) {
      rates[rateSpot++] = (byte)beatsPerMinute; // Almacenar esta lectura en el array
      rateSpot %= RATE_SIZE; // Envolver la variable
      // Tomar promedio de lecturas
      beatAvg = 0;
      for (byte x = 0; x < RATE_SIZE; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }

  // Comprobar si es tiempo de imprimir
  if (millis() - lastPrintTime > printInterval) {
    // Imprimir solo SensorValue y BPM
    Serial.print("SensorValue=");
    Serial.print(sensorValue);
    Serial.print(", BPM=");
    Serial.println(beatsPerMinute);

    lastPrintTime = millis(); // Actualizar la última vez que se imprimió la información
  }
}
