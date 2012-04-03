
#ifndef LIB_SERVO_H
#define LIB_SERVO_H

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

extern bool_t servo_query_status;

extern void servoInit(void);
extern void servoCommand(servo_msg_t *msgs, size_t len);

static inline void servoCommandOne(uint8_t channel, uint16_t width,
		uint16_t speed, uint16_t time)
{
	servo_msg_t msg = {
		.channel = channel,
		.width = width,
		.speed = speed,
		.time = time
	};

	servoCommand(&msg, 1);
}

#endif /* LIB_SERVO_H */
