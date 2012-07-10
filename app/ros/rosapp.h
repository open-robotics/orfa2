#ifndef APP_ROS_H
#define APP_ROS_H

#include "common.h"
#include "chprintf.h"

#ifdef __cplusplus
extern "C" {
#endif

	void appRos(BaseSequentialStream *chp, int argc, char *argv[]);

#ifdef __cplusplus
}; /* extern C */
#endif

#endif /* APP_ROS_H */
