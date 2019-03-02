#pragma once

#include <Adafruit_BMP280.h>

#include "Error.h"
#include "Pinout.h"

class BMP280_Sensor {
private:
	static float m_pressure;
	static float m_temperature;
	static Adafruit_BMP280 m_sensor;
public:
	static void setup();

	static void getPressure(float &pressure) { pressure = m_pressure; };
	static void getTemperature(float &temp) { temp = m_temperature; };
	static void pollSensor();
};