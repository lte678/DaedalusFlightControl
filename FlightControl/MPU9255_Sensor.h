#pragma once

#include <MPU9255.h>

#include "Error.h"

class MPU9255_Sensor {
private:
	static MPU9255 m_sensor;
public:
	static void setup();
	static void pollSensor();
	static void getAcceleration(float &x, float &y, float &z);
	static void getGyro(float &x, float &y, float &z);
	static void getCompass(float &x, float &y, float &z);
};