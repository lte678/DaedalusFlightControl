#include "BMP280_Sensor.h"

float BMP280_Sensor::m_pressure = 0.0f;
float BMP280_Sensor::m_temperature = 0.0f;
Adafruit_BMP280 BMP280_Sensor::m_sensor;

void BMP280_Sensor::setup() {
	if (!m_sensor.begin()) {
		g_Error.SENSOR_BMP280_INIT = true;
	}
}

void BMP280_Sensor::pollSensor() {
	if (!g_Error.SENSOR_BMP280_INIT) {
		m_pressure = m_sensor.readPressure();
		m_temperature = m_sensor.readTemperature();
	}
	else if (m_sensor.begin()) {
		g_Error.SENSOR_BMP280_INIT = false;
	}
}