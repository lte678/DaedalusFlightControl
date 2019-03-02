/*
 Name:		flightcontrol.ino
 Created:	1/31/2019 11:00:36 AM
 Author:	Leon
*/

// For the main message handling functionallity
#include "CommandParser.h"

// For all additional flight control and utility functions
#include "Util.h"

// Determines the max message / response length - 1
#define TX_RX_BUFFER_SIZE  120


// PROGRAM BEGIN / INITIALIZATION
void setup() {
	Serial.begin(9600);

	systemInit();
}


// EXECUTION THREAD
void loop() {
	static char input[TX_RX_BUFFER_SIZE];
	static char response[TX_RX_BUFFER_SIZE];
	static unsigned char inputIndex = 0;

	// Execute as regularly as possible!
	systemPoll();

	while (Serial.available()) {
		char c = Serial.read();
		if (c == '\n') {
			if (!g_Error.PARSER_INSUFFICIENT_BUFFER) {
				input[inputIndex] = 0;
				CommandParser::parse(input, response, TX_RX_BUFFER_SIZE);
				Serial.print(response);
			}
			else {
				Serial.print(F("No-Ack\n"));
				g_Error.PARSER_INSUFFICIENT_BUFFER = false;
			}
			inputIndex = 0;
		}
		else if (inputIndex < TX_RX_BUFFER_SIZE - 1) {
			input[inputIndex] = c;
			inputIndex++;
		}
		else {
			g_Error.PARSER_INSUFFICIENT_BUFFER = true;
		}
	}
}
