#include "CommandParser.h"

#include <HardwareSerial.h>

unsigned short CommandParser::allocated;
CommandParser::SavedMsg CommandParser::saved[MAX_SAVED_MSGS];
//char CommandParser::saved2[MAX_ALLOCATION];

const char pingTag[] PROGMEM = "ping";
const char echoTag[] PROGMEM = "echo";
const char loadTag[] PROGMEM = "load";
const char sortTag[] PROGMEM = "sort";
const char mathTag[] PROGMEM = "math";
const char invertTag[] PROGMEM = "invert";
const char saveTag[] PROGMEM = "save";
const char factorTag[] PROGMEM = "factor";
const char statusTag[] PROGMEM = "status";
const char sensorTag[] PROGMEM = "sensor";
const char ramTag[] PROGMEM = "ram";
const char errorsTag[] PROGMEM = "errors";
const char chuteTag[] PROGMEM = "para";

// The main command parser function
void CommandParser::parse(char *command, char *response, int bufferSize) {
	MessageType messageType;
	int messageNumber;
	char *parameter;
	response[0] = '\0';

	unpackMessage(command, messageType, messageNumber, &parameter);

	if (g_Error.PARSER_INVALID_CRC || g_Error.PARSER_INVALID_FORMAT) {
		messageType = MessageType::Invalid;
	}

	sprintf(response, "%i;", messageNumber);

	switch (messageType) {
	case MessageType::Ping:
		strncat_P(response, pingTag, bufferSize);
		strncat(response, ";", bufferSize);
		handlePing(response, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Echo:
		strncat_P(response, echoTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleEcho(response, parameter, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Sensor:
		strncat_P(response, sensorTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleSensor(response, parameter, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Status:
		strncat_P(response, statusTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleStatus(response, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Save:
		strncat_P(response, saveTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleSave(response, messageNumber, parameter, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Load:
		strncat_P(response, loadTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleLoad(response, parameter, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Ram:
		strncat_P(response, ramTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleRam(response, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Chute:
		strncat_P(response, chuteTag, bufferSize);
		strncat(response, ";;", bufferSize);
		handleChute();
		break;
	case MessageType::Math:
		strncat_P(response, mathTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleMath(response, parameter, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Factor:
		strncat_P(response, factorTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleFactor(response, parameter, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Sort:
		strncat_P(response, sortTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleSort(response, parameter, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Errors:
		strncat_P(response, errorsTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleErrors(response, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	case MessageType::Invert:
		strncat_P(response, invertTag, bufferSize);
		strncat(response, ";", bufferSize);
		handleInvert(response, parameter, bufferSize);
		strncat(response, ";", bufferSize);
		break;
	default:
		strncat_P(response, PSTR("No-Ack"), bufferSize);
		strncat(response, ";;", bufferSize);
	}

	unsigned char responseLen = strlen(response);
	snprintf(response + responseLen, bufferSize - responseLen, "%lu\n", calculateCRC(response, response + responseLen));
}

void CommandParser::unpackMessage(char *command, MessageType &messageType, int &messageNumber, char **parameterString) {
	unsigned char segment = 0;
	char *token = command;
	char *pos = strchr(command, ';');

	g_Error.PARSER_INVALID_FORMAT = false;

	if (!pos) {
		g_Error.PARSER_INVALID_FORMAT = true;
		return;
	}

	*pos = '\0';
	int i;

	while (token != nullptr) {
		switch (segment) {
		case 0:
			messageNumber = atoi(token);
			break;
		case 1:
			if (strcmp_P(token, pingTag) == 0) {
				messageType = MessageType::Ping;
			} else if (strcmp_P(token, echoTag) == 0) {
				messageType = MessageType::Echo;
			} else if (strcmp_P(token, sensorTag) == 0) {
				messageType = MessageType::Sensor;
			} else if (strcmp_P(token, statusTag) == 0) {
				messageType = MessageType::Status;
			} else if (strcmp_P(token, mathTag) == 0) {
				messageType = MessageType::Math;
			} else if (strcmp_P(token, factorTag) == 0) {
				messageType = MessageType::Factor;
			} else if (strcmp_P(token, saveTag) == 0) {
				messageType = MessageType::Save;
			} else if (strcmp_P(token, loadTag) == 0) {
				messageType = MessageType::Load;
			} else if (strcmp_P(token, sortTag) == 0) {
				messageType = MessageType::Sort;
			} else if (strcmp_P(token, invertTag) == 0) {
				messageType = MessageType::Invert;
			} else if (strcmp_P(token, ramTag) == 0) {
				messageType = MessageType::Ram;
			} else if (strcmp_P(token, errorsTag) == 0) {
				messageType = MessageType::Errors;
			} else if (strcmp_P(token, chuteTag) == 0) {
				messageType = MessageType::Chute;
			} else {
				messageType = MessageType::Invalid;
			}
			break;
		case 2:
			*parameterString = token;
			break;
		case 3:
			g_Error.PARSER_INVALID_CRC = !(calculateCRC(command, token) == atol(token));
			break;
		}

		if (pos != nullptr) {
			token = pos + 1;
		} else {
			token = nullptr;
		}
		pos = strchr(token, ';');
		if (pos) {
			*pos = '\0';
		}
		
		segment++;
	}
	
	if (segment != 4) {
		g_Error.PARSER_INVALID_FORMAT = true;
	}
}

unsigned long CommandParser::calculateCRC(const char *message, const char *end) {
	int i, j;
	unsigned long byte, crc, mask;

	i = 0;
	crc = 0xFFFFFFFF;
	while (&message[i] < end) {
		byte = message[i];          // Get next byte.
		if (!byte) byte = ';';		//Fix our string from the parser
		
		crc = crc ^ byte;
		for (j = 7; j >= 0; j--) {  // Do eight times.
			mask = -(crc & 1);
			crc = (crc >> 1) ^ (0xEDB88320 & mask);
		}
		i = i + 1;
	}
	return ~crc;
}

//
// Matrix inversion helper functions
//
int CommandParser::ind(unsigned char row, unsigned char column) { return row * 3 + column; }

int CommandParser::detMat2(int a, int b, int c, int d) {
	return a * d - b * c;
}

int CommandParser::detMat3(int *m) {
	return m[ind(0, 0)] * detMat2(m[ind(1, 1)], m[ind(1, 2)], m[ind(2, 1)], m[ind(2, 2)]) -
		m[ind(0, 1)] * detMat2(m[ind(1, 0)], m[ind(1, 2)], m[ind(2, 0)], m[ind(2, 2)]) +
		m[ind(0, 2)] * detMat2(m[ind(1, 0)], m[ind(1, 1)], m[ind(2, 0)], m[ind(2, 1)]);
}

//
// Here all of the different response handlers are defined
//

void CommandParser::handlePing(char *response, int buffersize) {
	//Do nothing, just send an empty frame back
}

void CommandParser::handleSort(char *response, const char *parameter, int buffersize) {
	//Unpack list
	int list[MAX_SORT_ELEMENTS];
	unsigned char idx = 0;
	for (unsigned int i = 0; i < strlen(parameter); i++) {
		if (parameter[i] == '[' || parameter[i] == ' ') {
			sscanf(parameter + i + 1, "%i", &list[idx]);
			idx++;
		}
	}

	//Sort list
	for (char i = 1; i < idx; i++) {
		int curr = list[i];
		char j = i - 1;
		while (list[j] > curr && j >= 0) {
			list[j + 1] = list[j];
			j--;
		}
		list[j + 1] = curr;
	}

	//Repack list
	strncat(response, "[", buffersize);
	for (unsigned char i = 0; i < idx; i++) {
		snprintf_P(response + strlen(response), buffersize - strlen(response), PSTR("%i"), list[i]);
		if(i < idx - 1) strncat(response, ", ", buffersize);
	}
	strncat(response, "]", buffersize);
}

void CommandParser::handleMath(char *response, const char *parameter, int buffersize) {
	int inA, inB;
	float a, b;
	char op = '\0';
	sscanf_P(parameter, PSTR("%u %c %u"), &inA, &op, &inB);
	a = inA;
	b = inB;
	if (op == '+') { a = a + b; }
	else if (op == '-') { a = a - b; }
	else if (op == '*') { a = a * b; }
	else if (op == '/') { a = a / b; }
	else { return; }

	char floatStr[12];
	dtostrf(a, 0, 2, floatStr);
	//We want to concat to response
	strncat(response, floatStr, buffersize);
}

void CommandParser::handleInvert(char *response, const char *param, int buffersize) {
	// Unpack matrix
	int matrix[9];
	unsigned char idx = 0;
	int read = false; // Was the current sequence of numbers already read?
	for (unsigned int i = 0; i < strlen(param); i++) {
		if (param[i] >= '0' && param[i] <= '9' || param[i] == '-' ) {
			if (idx < 9 && !read) {
				read = true;
				sscanf(param + i, "%i", &matrix[idx]);
				idx++;
			}
		}
		else {
			read = false;
		}
	}

	float det = detMat3(matrix);

	char floatStr[12];
	strncat(response, "[", buffersize);
	for (unsigned char i = 0; i < 3; i++) {
		strncat(response, "[", buffersize);
		for (unsigned char j = 0; j < 3; j++) {
			unsigned char r1, r2, c1, c2;
			switch (i) {
			case 0:
				r1 = 1;
				r2 = 2;
				break;
			case 1:
				r1 = 0;
				r2 = 2;
				break;
			default:
				r1 = 0;
				r2 = 1;
			}
			
			switch (j) {
			case 0:
				c1 = 1;
				c2 = 2;
				break;
			case 1:
				c1 = 0;
				c2 = 2;
				break;
			default:
				c1 = 0;
				c2 = 1;
			}

			float inv = detMat2(matrix[ind(c1, r1)], matrix[ind(c1, r2)], matrix[ind(c2, r1)], matrix[ind(c2, r2)]) / det;

			inv =  (i + j) % 2 ? -inv : inv;

			dtostrf(inv, 0, 2, floatStr);
			strncat(response, floatStr, buffersize);
			if (j < 2) strncat(response, ", ", buffersize);
		}
		strncat(response, "]", buffersize);
		if (i < 2) strncat(response, ", ", buffersize);
	}
	strncat(response, "]", buffersize);
}

void CommandParser::handleEcho(char *response, const char *parameter, int buffersize) {
	strncat(response, parameter, buffersize);
}

void CommandParser::handleSave(char *response, int msgNumber, char *msg, int buffersize) {
	unsigned int msgLength = strlen(msg) + 1;
	if (allocated + msgLength <= MAX_ALLOCATION) {
		g_Error.PARSER_SAVE_DATA_SIZE = false;
		for (int i = 0; i < MAX_SAVED_MSGS; i++) {
			if (saved[i].msgNumber == -1) {
				g_Error.PARSER_SAVE_MAX_PACKETS = false;
				saved[i].data = (char*)malloc(msgLength);
				if (saved[i].data == nullptr) {
					g_Error.PARSER_SAVE_NO_MEMORY = true;
				}
				else {
					g_Error.PARSER_SAVE_NO_MEMORY = false;
					strcpy(saved[i].data, msg);
					saved[i].msgNumber = msgNumber;
					allocated += msgLength;
				}
				return;
			}
		}
		g_Error.PARSER_SAVE_MAX_PACKETS = true;
	}
	else {
		g_Error.PARSER_SAVE_DATA_SIZE = true;
	}

}

void CommandParser::handleLoad(char *response, const char *param, int buffersize) {
	int msgNumber = atoi(param);
	if (msgNumber < 0) return;

	for (int i = 0; i < MAX_SAVED_MSGS; i++) {
		if (saved[i].msgNumber == msgNumber) {
			strncat(response, saved[i].data, buffersize);
			allocated -= strlen(saved[i].data) + 1;
			free(saved[i].data);
			saved[i].msgNumber = -1;
			return;
		}
	}
}

void CommandParser::handleStatus(char *response, int buffersize) {
	strncat_P(response, flightStatus(), buffersize);
}

void CommandParser::handleSensor(char *response, char *sensor, int buffersize) {
	sensorJSON(response + strlen(response), sensor, buffersize - strlen(response)); // Puts all the sensor data into response
}

void CommandParser::handleFactor(char *response, const char *parameter, int buffersize) {
	long num = atol(parameter);
	long num_1 = 0l;
	long num_2 = 0l;
	while (num % 2 == 0) {
		num /= 2;
		if (num_1 != 0) {
			num_2 = 2;
		}
		else {
			num_1 = 2;
		}
	}

	long limit = sqrt(num);
	for (unsigned long i = 3; i <= limit; i += 2) {
		while (num % i == 0) {
			num /= i;
			if (num_1 != 0) {
				num_2 = i;
			}
			else {
				num_1 = i;
			}
		}
		systemPoll(); //Make sure we're not missing any sensor reads
	}

	if (num > 2) {
		if (num_1 != 0) {
			num_2 = num;
		}
		else {
			num_1 = num;
		}
	}

	snprintf_P(response + strlen(response), buffersize - strlen(response), PSTR("%ld,%ld"), num_1, num_2);
}

void CommandParser::handleRam(char *response, int buffersize) {
	sprintf(response + strlen(response), "%i", freeRam());
}

void CommandParser::handleErrors(char *response, int buffersize) {
	sprintf_P(response + strlen(response), PSTR("{\"errors\":0x%04X}"), g_Error);
}

void CommandParser::handleChute() {
	deployChute(true);
}