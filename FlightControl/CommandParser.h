#pragma once

#include <string.h>
#include <stdlib.h>

#include "MPU9255_Sensor.h"
#include "BMP280_Sensor.h"

#include "Error.h"
#include "Util.h"

#define MAX_ALLOCATION 50
#define MAX_SAVED_MSGS 2
#define MAX_SORT_ELEMENTS 10

class CommandParser {
public:
	static void parse(char *command, char *response, int bufferSize);
private:
	enum MessageType {
		Invalid,
		Ping,
		Sort,
		Math,
		Invert,
		Echo,
		Save,
		Load,
		Status,
		Sensor,
		Factor,
		Ram,
		Errors,
		Chute
	};

	struct SavedMsg {
		short msgNumber = -1;
		char* data;
	};

	static unsigned short allocated; //The current number of allocated bytes
	static SavedMsg saved[MAX_SAVED_MSGS]; //Points to locations in saved2 containing the data
	//static char saved2[MAX_ALLOCATION]; // Storage for the actual data

	static void unpackMessage(char* command, MessageType &messageType, int &messageNumber, char **parameterString);
	static unsigned long calculateCRC(const char *message, const char *end);

	static int ind(unsigned char row, unsigned char column);
	static int detMat2(int a, int b, int c, int d);
	static int detMat3(int *matrix);

	static void handlePing(char *response, int buffersize);
	static void handleSort(char *response, const char *parameter, int buffersize);
	static void handleMath(char *response, const char *parameter, int buffersize);
	static void handleInvert(char *response, const char *param, int buffersize);
	static void handleEcho(char *response, const char *parameter, int buffersize);
	static void handleSave(char *response, int msgNumber, char *msg, int buffersize);
	static void handleLoad(char *response, const char* param, int buffersize);
	static void handleStatus(char *response, int buffersize);
	static void handleSensor(char *response, char *sensor, int buffersize);
	static void handleFactor(char *response, const char *parameter, int buffersize);
	static void handleRam(char *response, int buffersize);
	static void handleErrors(char *response, int buffersize);
	static void handleChute();
};