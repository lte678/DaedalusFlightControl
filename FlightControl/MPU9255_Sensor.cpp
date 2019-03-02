#include "MPU9255_Sensor.h"

MPU9255 MPU9255_Sensor::m_sensor;

void MPU9255_Sensor::setup() {
	if (m_sensor.init()) {
		g_Error.SENSOR_MPU9255_INIT = true;
	}
	else {
		m_sensor.set_acc_scale(scale_4g);
		m_sensor.set_gyro_scale(scale_500dps);
	}
}

void MPU9255_Sensor::pollSensor() {
	if (!g_Error.SENSOR_MPU9255_INIT) {
		m_sensor.read_acc();
		m_sensor.read_gyro();
		m_sensor.read_mag();
	}
	else if (!m_sensor.init()) {
		g_Error.SENSOR_MPU9255_INIT = false;
		m_sensor.set_acc_scale(scale_4g);
		m_sensor.set_gyro_scale(scale_500dps);
	}
}

void MPU9255_Sensor::getAcceleration(float &x, float &y, float &z) {
	x = m_sensor.ax;
	x /= 8192;
	y = m_sensor.ay;
	y /= 8192;
	z = m_sensor.az;
	z /= 8192;
}

void MPU9255_Sensor::getGyro(float &x, float &y, float &z) {
	x = m_sensor.gx;
	x /= 65.5f;
	y = m_sensor.gy;
	y /= 65.5f;
	z = m_sensor.gz;
	z /= 65.5f;
}

void MPU9255_Sensor::getCompass(float &x, float &y, float &z) {
	x = m_sensor.mx;
	y = m_sensor.my;
	z = m_sensor.mz;
}