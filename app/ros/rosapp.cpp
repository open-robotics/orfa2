
#include <ros.h>
#include "rosapp.h"

#include <std_msgs/UInt16.h>

#include "lib/servo.h"

ros::NodeHandle nh;

std_msgs::UInt16 pos_msg;
ros::Publisher servo_state("servo_state", &pos_msg);

void servo_cb(const std_msgs::UInt16& cmd_msg)
{
	servoCommandOne(12, cmd_msg.data, 100, 0);
}

ros::Subscriber<std_msgs::UInt16> servo_cmd("servo_cmd", servo_cb);

void appRos(BaseChannel *chp, int argc, char *argv[])
{

	(void)argc;
	(void)argv;

	nh.initNode();
	nh.advertise(servo_state);
	nh.subscribe(servo_cmd);

	while (!chThdShouldTerminate()) {
		nh.spinOnce();

		/* TODO VTimer */
		pos_msg.data = rcsGetWidth(&RCSD1, 12);
		servo_state.publish(&pos_msg);
	}
}

