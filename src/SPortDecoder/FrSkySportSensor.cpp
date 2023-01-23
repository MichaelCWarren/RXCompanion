/*
  FrSky sensor base class for Teensy LC/3.x/4.x, ESP8266, ATmega2560 (Mega) and ATmega328P based boards (e.g. Pro Mini, Nano, Uno)
  (c) Pawelsky 20210509
  Not for commercial use
*/

#include "FrSkySportSensor.h" 

FrSkySportSensor::FrSkySportSensor(SensorId id) : sensorId(id), sensorDataIdx(0) {}
uint16_t FrSkySportSensor::decodeData(uint8_t id, uint16_t appId, uint32_t data) { return SENSOR_NO_DATA_ID; }
