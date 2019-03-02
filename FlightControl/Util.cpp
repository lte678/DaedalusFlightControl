#include "Util.h"

bool g_ParachuteDeployed = false;
FlightStatus g_Status;
#ifdef USE_LOGGING_SDFAT
SdFat sdCard;
SdFile logFile;
#endif

void pollSensors() {
	MPU9255_Sensor::pollSensor();
	BMP280_Sensor::pollSensor();
}

//Takes about 16 - 20 ms to run (as of now)
void systemPoll() {
	static unsigned long lastPoll = 0;
	if (millis() - lastPoll > POLL_INTERVAL) {
		pollSensors();
		deployChute(false); // Just update its state
		pollFlightStatus();
		logData();
		lastPoll += POLL_INTERVAL;
	}
}

void pollFlightStatus() {
	switch (g_Status) {
	case FlightStatus::Idle:
		//Starting state
		break;
	case FlightStatus::Ascend:
		break;
	case FlightStatus::Descend:
		break;
	case FlightStatus::Landed:
		//Final state, this can never progress
		break;
	}
}

void sensorJSON(char *json, const char *sensor, int bufferSize) {
	if (strcmp_P(sensor, sensorAccelerometerTag) == 0) {
		float x, y, z;
		MPU9255_Sensor::getAcceleration(x, y, z);
		x *= 1000.0f;
		y *= 1000.0f;
		z *= 1000.0f;
		snprintf_P(json, bufferSize, PSTR("{acc:[{x:%i},{y:%i},{z:%i}]}"), int(x), (int)y, (int)z);
	}
	else if (strcmp_P(sensor, sensorGyrometerTag) == 0) {
		float x, y, z;
		MPU9255_Sensor::getGyro(x, y, z);
		snprintf_P(json, bufferSize, PSTR("{gyr:[{x:%i},{y:%i},{z:%i}]}"), int(x), (int)y, (int)z);
	}
	else if (strcmp_P(sensor, sensorMagneticTag) == 0) {
		float x, y, z;
		MPU9255_Sensor::getCompass(x, y, z);
		snprintf_P(json, bufferSize, PSTR("{mag:[{x:%i},{y:%i},{z:%i}]}"), int(x), (int)y, (int)z);
	}
	else if (strcmp_P(sensor, sensorAtmosphericTag) == 0) {
		float pressure, temperature;
		BMP280_Sensor::getPressure(pressure);
		BMP280_Sensor::getTemperature(temperature);
		pressure /= 100.0f;
		temperature *= 1000;
		snprintf_P(json, bufferSize, PSTR("{atm:[{temp:%i},{press:%i}]}"), int(temperature), int(pressure));
	}
	else {
		// Looks like we just want to complete the default task..
		char tempStr[8];
		float pressure, temperature;
		BMP280_Sensor::getPressure(pressure);
		pressure /= 100.0f;
		BMP280_Sensor::getTemperature(temperature);
		if (g_Error.SENSOR_BMP280_INIT) {
			strcpy_P(tempStr, PSTR("err"));
		} else if(temperature < 1000.0f && temperature > -100.0f) {
			dtostrf(temperature, 0, 1, tempStr);
		} else {
			strcpy_P(tempStr, PSTR("nan"));
		}
		snprintf_P(json, bufferSize, PSTR("%d,%s"), (int)pressure, tempStr);
	}
}

void initLog() {
	#ifdef USE_LOGGING_SDFAT
	if (!sdCard.begin()) {
		g_Error.LOG_OPEN_FILE = true;
		return;
	}
	if (!logFile.open("log.txt", O_WRITE | O_APPEND | O_CREAT)) {
		g_Error.LOG_OPEN_FILE = true;
		return;
	}

	logFile.println(F("--- Log Start ---"));
	#endif
}

void logData() {
	#ifdef USE_LOGGING_SDFAT
	static char logText[LOG_BUFFER_SIZE];
	if (!g_Error.LOG_OPEN_FILE) {
		snprintf_P(logText, LOG_BUFFER_SIZE, PSTR("${t:%lu,errs:0x%04X,status:%S}"), millis(), g_Error, flightStatus());
		logFile.println(logText);
		snprintf_P(logText, LOG_BUFFER_SIZE, PSTR("{vbat:%d,chute:%d}"), readBatVoltage(), g_ParachuteDeployed);
		logFile.println(logText);
		sensorJSON(logText, "acc", LOG_BUFFER_SIZE);
		logFile.println(logText);
		sensorJSON(logText, "mag", LOG_BUFFER_SIZE);
		logFile.println(logText);
		sensorJSON(logText, "gyr", LOG_BUFFER_SIZE);
		logFile.println(logText);
		sensorJSON(logText, "atm", LOG_BUFFER_SIZE);
		logFile.println(logText);
		logFile.sync();
	}
	#endif
}

void systemInit() {
	MPU9255_Sensor::setup();
	BMP280_Sensor::setup();
	pinMode(CHUTE_DEPLOY_PIN, OUTPUT);
	initLog();
}

int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void deployChute(bool deploy) {
	static bool deploying;
	static unsigned long deployTime;
	if (deploy) {
		deploying = true;
		deployTime = millis();
		digitalWrite(CHUTE_DEPLOY_PIN, HIGH);
	}
	else if (deploying && millis() > deployTime + CHUTE_DEPLOY_TIME) {
		deploying = false;
		g_ParachuteDeployed = true;
		digitalWrite(CHUTE_DEPLOY_PIN, LOW);
	}
}

int readBatVoltage() {
	// Return lipo voltage in mV
	return analogRead(VBAT_PIN) * (5000.0f / 1023.0f);
}

const char* flightStatus() {
	switch (g_Status) {
	case FlightStatus::Idle:
		return statusIdle;
	case FlightStatus::Ascend:
		return statusAscend;
	case FlightStatus::Descend:
		return statusDescend;
	case FlightStatus::Landed:
		return statusLanded;
	}
}