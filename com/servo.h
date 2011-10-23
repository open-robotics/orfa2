
#ifndef CSERVO_H
#define CSERVO_H

#include "ch.h"
#include "ohal.h"

typedef struct servo_msg {
	uint8_t		channel;
	uint16_t	width;
	uint16_t	speed;
} servo_msg_t;

#define SERVO_TIME_CH	UINT8_MAX
#define SERVO_EVT_DONE	1

extern BaseAsynchronousChannel servo_cmd;

extern void servoInit();

#endif /* CSERVO_H */
