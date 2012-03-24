
#ifndef CSERVO_H
#define CSERVO_H

#include "ch.h"
#include "rhal.h"

typedef struct servo_msg {
	uint8_t		channel;
	uint16_t	width;
	uint16_t	speed;
	uint16_t	time;
} servo_msg_t;

#define SERVO_SPEED_MAX	UINT16_MAX
#define SERVO_TIME_MAX	UINT16_MAX

#define SERVO_QUERY_DONE	FALSE
#define SERVO_QUERY_IN_PROGRESS	TRUE

extern BaseAsynchronousChannel servo_cmd;
extern bool_t servo_query_status;

extern void servoInit();

#endif /* CSERVO_H */
