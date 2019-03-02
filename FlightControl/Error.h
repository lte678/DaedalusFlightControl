#pragma once

struct CansatError {
	byte PARSER_INVALID_CRC : 1; // The checksum of the received message was invalid
	byte PARSER_INSUFFICIENT_BUFFER : 1; // A parser buffer overflowed
	byte PARSER_INVALID_FORMAT : 1; //The received command was not delimeted correctly
	byte PARSER_SAVE_DATA_SIZE : 1; // The max amount of memory to allocate to save operations has been surpassed
	byte PARSER_SAVE_MAX_PACKETS : 1; // The max amount of saved messages has been surpassed
	byte PARSER_SAVE_NO_MEMORY : 1; // If malloc fails to return 
	byte SENSOR_BMP280_INIT : 1; // The pressure / temperature sensor could not be initialized
	byte SENSOR_MPU9255_INIT : 1; // The 9-DOF module couldn't be initialized 
	byte LOG_INIT_SD : 1; // Failed to initialize the SD card
	byte LOG_OPEN_FILE : 1; // The logger failed to open the log file
};

static CansatError g_Error;