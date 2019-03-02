#pragma once

#define USE_LOGGING_SDFAT

#ifdef USE_LOGGING_SDFAT
#include <SdFat.h>
#endif

#include "BMP280_Sensor.h"
#include "MPU9255_Sensor.h"
#include "avr/pgmspace.h"

#define POLL_INTERVAL 500
#define LOG_BUFFER_SIZE 40
#define CHUTE_DEPLOY_TIME 400

const char sensorAccelerometerTag[] PROGMEM = "acc";
const char sensorGyrometerTag[] PROGMEM = "gyr";
const char sensorMagneticTag[] PROGMEM = "mag";
const char sensorAtmosphericTag[] PROGMEM = "atm";

const char statusIdle[] PROGMEM = "idle";
const char statusAscend[] PROGMEM = "ascend";
const char statusDescend[] PROGMEM = "descend";
const char statusLanded[] PROGMEM = "landed";

enum FlightStatus {
	Idle,
	Ascend,
	Descend,
	Landed
};

extern bool g_ParachuteDeployed;
extern FlightStatus g_Status;
#ifdef USE_LOGGING_SDFAT
extern SdFat sdCard;
extern SdFile logFile;
#endif

void systemInit();
void pollSensors();
void pollFlightStatus();
void systemPoll();

void sensorJSON(char *json, const char *sensor, int bufferSize);
const char* flightStatus();

void initLog();
void logData();
int readBatVoltage();

void deployChute(bool deploy);

int freeRam();