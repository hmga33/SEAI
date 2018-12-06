#ifndef ARDUINO_TYPES_H
#define ARDUINO_TYPES_H

#ifndef ARDUINOLINK_MAX_PAYLOAD_LEN
#define ARDUINOLINK_MAX_PAYLOAD_LEN 255
#endif

typedef struct __arduino_message {
	uint8_t id; //id message
	uint8_t *buf; //message
} arduino_message_t;

typedef enum {
	MSG_INTERNAL_TEMPERATURE	= 10,
	MSG_WATER_TEMPERATURE		= 11,
	MSG_GPS				= 20,
	MSG_IMU				= 30,	
} arduino_message_type_t;

#endif
