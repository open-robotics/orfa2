
#include <ros.h>
#include "rosapp.h"

void appRos(BaseChannel *chp, int argc, char *argv[])
{
	ros::NodeHandle nh;

	(void)argc;
	(void)argv;

	nh.initNode();

	while (!chThdShouldTerminate()) {
		nh.spinOnce();
	}
}

