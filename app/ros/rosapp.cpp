
#include "common.h"
#include "evtimer.h"
#include "rosapp.h"
#include "servo.h"

#include <ros.h>
#include <orfa2_msgs/ServoState.h>


void servo_cmd_cb(const orfa2_msgs::ServoState& cmd_msg);

EvTimer servo_et;

ros::NodeHandle nh;

orfa2_msgs::ServoState servo_st_msg;
ros::Publisher servo_state("orfa/servo/state", &servo_st_msg);
ros::Subscriber<orfa2_msgs::ServoState> servo_cmd("orfa/servo/cmd", servo_cmd_cb);

void servo_cmd_cb(const orfa2_msgs::ServoState& cmd_msg)
{
	servo_msg_t cmds[RCS_CHANNELS];
	size_t n = 0;

	if (cmd_msg.servo_length != cmd_msg.position_length ||
			cmd_msg.servo_length != cmd_msg.velocity_length) {
		nh.logerror("servo_cmd: different array dimensions");
		return;
	}

	for (int i = 0; i < cmd_msg.servo_length && n < RCS_CHANNELS; i++) {
		if (cmd_msg.servo[i] < RCS_CHANNELS) {
			cmds[n].channel = cmd_msg.servo[i];
			cmds[n].width = cmd_msg.position[i];
			cmds[n].speed = cmd_msg.velocity[i];
			cmds[n++].time = 0;
		}
	}

	servoCommand(cmds, n);
}

void appRos(BaseChannel *chp, int argc, char *argv[])
{
	EventListener el0;

	(void)argc;
	(void)argv;

	evtInit(&servo_et, MS2ST(100));

	chEvtRegister(&servo_et.et_es, &el0, 0);

	nh.initNode();
	nh.advertise(servo_state);
	nh.subscribe(servo_cmd);

	evtStart(&servo_et);

	while (!chThdShouldTerminate()) {
		eventmask_t m = chEvtWaitOneTimeout(ALL_EVENTS, MS2ST(1));
		if (m & EVENT_MASK(0)) {
			uint8_t servos[RCS_CHANNELS];
			uint16_t positions[RCS_CHANNELS];
			uint16_t velocities[RCS_CHANNELS];
			size_t n = 0;

			for (int i = 0; i < RCS_CHANNELS; i++) {
				if (rcsGetWidth(&RCSD1, i) != 0) {
					servos[n] = i;
					positions[n] = rcsGetWidth(&RCSD1, i);
					velocities[n] = 0;
					n++;
				}
			}

			servo_st_msg.servo = servos;
			servo_st_msg.servo_length = n;
			servo_st_msg.position = positions;
			servo_st_msg.position_length = n;
			servo_st_msg.velocity = velocities;
			servo_st_msg.velocity_length = n;
			servo_st_msg.header.stamp = nh.now();

			if (n > 0)
				servo_state.publish(&servo_st_msg);
		}

		nh.spinOnce();
	}
}

