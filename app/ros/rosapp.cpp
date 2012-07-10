
#include "common.h"
#include "evtimer.h"
#include "rosapp.h"
#include "servo.h"
#include "palmap.h"

#include <ros.h>
#include <orfa2_msgs/ServoState.h>
#include <orfa2_msgs/Analog.h>
#include <orfa2_msgs/Digital.h>
#include <orfa2_msgs/SetupChannel.h>
#include <orfa2_msgs/Motor.h>
#include <orfa2_msgs/SetupMotor.h>
#include <roscpp/Empty.h>


static EvTimer servo_et, din_et;
static uint8_t pin_mode[PALMAP_PADS_SIZE];
static uint8_t motor_mode[DCM_CHANNELS];
static int8_t enc_to_motor[2];
static ros::Duration motor_int_to, motor_md2_to; /* timeout */
static ros::Time motor_int_htime, motor_md2_htime; /* last Header.stamp */

/* orfa2_msgs::SetupChannel::Request::CONST */
#define INPUT		0
#define OUTPUT		1
#define ANALOG_IN	2

/* orfa2_msgs::SetupMotor::Request::CONST */
#define MODE_OPEN_LOOP			0
#define MODE_POSITION_CONTROL	1
#define MODE_SPEED_CONTROL		2

#ifdef SIMULATOR
#define PAL_MODE_INPUT_PULLUP PAL_MODE_INPUT
#define PAL_MODE_INPUT_ANALOG PAL_MODE_INPUT
#endif

/*
 * ROS topics
 */

void servo_cmd_cb(const orfa2_msgs::ServoState & cmd_msg);
void digital_out_cb(const orfa2_msgs::Digital & cmd_msg);
void motor_int_cb(const orfa2_msgs::Motor & cmd_msg);
void motor_md2_cb(const orfa2_msgs::Motor & cmd_msg);
void stop_cb(const roscpp::Empty::Request & req, roscpp::Empty::Response & res);
void setup_channel_cb(const orfa2_msgs::SetupChannel::Request & req, orfa2_msgs::SetupChannel::Response & res);
void setup_motor_cb(const orfa2_msgs::SetupMotor::Request & req, orfa2_msgs::SetupMotor::Response & res);

ros::NodeHandle nh;

orfa2_msgs::ServoState servo_st_msg;
ros::Publisher servo_state("~servo/state", &servo_st_msg);
ros::Subscriber<orfa2_msgs::ServoState> servo_cmd("~servo/cmd", servo_cmd_cb);

orfa2_msgs::Analog adc_st_msg;
ros::Publisher analog_in("~io/analog_in", &adc_st_msg);

orfa2_msgs::Digital din_st_msg;
ros::Publisher digital_in("~io/digital_in", &din_st_msg);
ros::Subscriber<orfa2_msgs::Digital> digital_out("~io/digital_out", digital_out_cb);

ros::Subscriber<orfa2_msgs::Motor> motor_int("~motor/integrated", motor_int_cb);
ros::Subscriber<orfa2_msgs::Motor> motor_md2("~motor/robomd2", motor_md2_cb);
/* TODO: motor status publisher */

ros::ServiceServer<roscpp::Empty::Request, roscpp::Empty::Response> stop_srv("~stop", stop_cb);
ros::ServiceServer<orfa2_msgs::SetupChannel::Request, orfa2_msgs::SetupChannel::Response> setup_ch_srv("~io/setup_channel", setup_channel_cb);
ros::ServiceServer<orfa2_msgs::SetupMotor::Request, orfa2_msgs::SetupMotor::Response> setup_motor_srv("~motor/setup_channel", setup_motor_cb);

/*
 * ROS Callbacks
 */

void servo_cmd_cb(const orfa2_msgs::ServoState& cmd_msg)
{
	servo_msg_t cmds[RCS_CHANNELS];
	size_t n = 0;

	if (cmd_msg.servo_length != cmd_msg.position_length ||
			cmd_msg.servo_length != cmd_msg.velocity_length) {
		nh.logerror("~servo/cmd: different array dimensions");
		return;
	}

	for (int i = 0; i < cmd_msg.servo_length && n < RCS_CHANNELS; i++) {
		if (cmd_msg.servo[i] < RCS_CHANNELS) {
			if (cmd_msg.position[i] != 0) {
				cmds[n].channel = cmd_msg.servo[i];
				cmds[n].width = cmd_msg.position[i];
				cmds[n].speed = cmd_msg.velocity[i];
				cmds[n++].time = 0;
			}
			else
				rcsDisableChannel(&RCSD1, cmd_msg.servo[i]);
		}
	}

	servoCommand(cmds, n);
}

void digital_out_cb(const orfa2_msgs::Digital& cmd_msg)
{
	if (cmd_msg.pin_length != cmd_msg.value_length) {
		nh.logerror("~io/digital_out: different array dimensions");
		return;
	}

	for (int i = 0; i < cmd_msg.pin_length; i++) {
		if (cmd_msg.pin[i] < PALMAP_PADS_SIZE) {
			pmDigitalWrite(cmd_msg.pin[i], cmd_msg.value[i]);
		}
	}
}

void motor_int_cb(const orfa2_msgs::Motor & cmd_msg)
{
	dcmwidth_t pw0, pw1;

	motor_int_htime = cmd_msg.header.stamp;

	pw0 = cmd_msg.value[0];
	pw1 = cmd_msg.value[1];

	dcmEnableChannel(&DCMD1, 0, pw0);
	dcmEnableChannel(&DCMD1, 1, pw1);
}

void motor_md2_cb(const orfa2_msgs::Motor & cmd_msg)
{
	dcmwidth_t pw2, pw3;

	motor_md2_htime = cmd_msg.header.stamp;

	pw2 = cmd_msg.value[0];
	pw3 = cmd_msg.value[1];

	dcmEnableChannel(&DCMD1, 2, pw2);
	dcmEnableChannel(&DCMD1, 3, pw3);
}

void stop_cb(const roscpp::Empty::Request & req, roscpp::Empty::Response & res)
{
	servoStopAll();

	dcmDisableChannel(&DCMD1, 0);
	dcmDisableChannel(&DCMD1, 1);
	dcmDisableChannel(&DCMD1, 2);
	dcmDisableChannel(&DCMD1, 3);
}

void setup_channel_cb(const orfa2_msgs::SetupChannel::Request & req, orfa2_msgs::SetupChannel::Response & res)
{
	bool_t adc_en = FALSE;

	res.result = 0;
	if (req.pin >= PALMAP_PADS_SIZE)
		return;

	switch (req.type) {
	case INPUT:
		pmSetMode(req.pin, PAL_MODE_INPUT_PULLUP);
		if (req.rate_ms > 0)
			din_et.et_interval = MS2ST(req.rate_ms);
		break;

	case OUTPUT:
		pmSetMode(req.pin, PAL_MODE_OUTPUT_PUSHPULL);
		break;

	case ANALOG_IN:
		if (pmGetAdcChannel(req.pin) != -1) {
			pmSetMode(req.pin, PAL_MODE_INPUT_ANALOG);
			adc_en = TRUE;
			if (req.rate_ms > 0)
				pmAnalogSetRate(req.rate_ms);
		}
		else
			return;
		break;

	default:
		return;
	};

	pin_mode[req.pin] = req.type;
	pmAnalogStart(req.pin, adc_en);
	res.result = 1;
}

void setup_motor_cb(const orfa2_msgs::SetupMotor::Request & req, orfa2_msgs::SetupMotor::Response & res)
{
	res.result = 0;

	if (req.channel == 0 || req.channel == 1) {
		motor_int_to = req.timeout;
		motor_int_htime = nh.now();
		res.result = 1;
	}
	else if (req.channel == 2 || req.channel == 3) {
		motor_md2_to = req.timeout;
		motor_md2_htime = nh.now();
		res.result = 1;
	}

	/* TODO */
}

/*
 * main thread
 */

void servo_state_ev(eventid_t m)
{
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

void din_ev(eventid_t m)
{
	uint8_t pins[PALMAP_PADS_SIZE];
	bool values[PALMAP_PADS_SIZE];
	size_t n = 0;

	for (int i = 0; i < PALMAP_PADS_SIZE; i++) {
		if (pin_mode[i] == INPUT) {
			pins[n] = i;
			values[n] = pmDigitalRead(i);
			n++;
		}
	}

	din_st_msg.pin = pins;
	din_st_msg.pin_length = n;
	din_st_msg.value = values;
	din_st_msg.value_length = n;
	din_st_msg.header.stamp = nh.now();

	if (n > 0)
		digital_in.publish(&din_st_msg);
}

void adc_done_ev(eventid_t m)
{
	uint8_t pins[PALMAP_PADS_SIZE]; /* XXX */
	uint16_t values[PALMAP_PADS_SIZE];
	size_t n = 0;

	for (int i = 0; i < PALMAP_PADS_SIZE; i++) {
		if (pin_mode[i] == ANALOG_IN) {
			pins[n] = i;
			values[n] = pmAnalogReadLast(i);
			n++;
		}
	}

	adc_st_msg.pin = pins;
	adc_st_msg.pin_length = n;
	adc_st_msg.value = values;
	adc_st_msg.value_length = n;
	adc_st_msg.header.stamp = nh.now();

	if (n > 0)
		analog_in.publish(&adc_st_msg);
}

void appRos(BaseSequentialStream *chp, int argc, char *argv[])
{
	ros::Time now, timeout;
	EventListener el0, el1, el2;
	evhandler_t handlers[] = {
		servo_state_ev,
		din_ev,
		adc_done_ev
	};

	(void)argc;
	(void)argv;

	evtInit(&servo_et, MS2ST(100));
	evtInit(&din_et, MS2ST(100));

	chEvtRegister(&servo_et.et_es, &el0, 0);
	chEvtRegister(&din_et.et_es, &el1, 1);
	chEvtRegister(pmAnalogDoneEvent(), &el2, 2);

	memset(pin_mode, INPUT, sizeof(pin_mode));

	nh.getHardware()->init((BaseChannel*)chp);
	nh.initNode();
	nh.advertise(servo_state);
	nh.subscribe(servo_cmd);
	nh.advertise(analog_in);
	nh.advertise(digital_in);
	nh.subscribe(digital_out);
	nh.subscribe(motor_int);
	nh.subscribe(motor_md2);
	nh.advertiseService(stop_srv);
	nh.advertiseService(setup_ch_srv);
	nh.advertiseService(setup_motor_srv);

	evtStart(&servo_et);
	evtStart(&din_et);

	while (!chThdShouldTerminate()) {
		chEvtDispatch(handlers, chEvtWaitOneTimeout(ALL_EVENTS, MS2ST(1)));

		nh.spinOnce();

		now = nh.now();
		timeout = motor_int_htime; timeout += motor_int_to;
		if (motor_int_to.sec &&
				(timeout.sec < now.sec ||
				 (timeout.sec == now.sec && timeout.nsec < now.nsec))) {
			/* internal watchdog */
			dcmDisableChannel(&DCMD1, 0);
			dcmDisableChannel(&DCMD1, 1);
		}

		timeout = motor_md2_htime; timeout += motor_md2_to;
		if (motor_md2_to.sec &&
				(timeout.sec < now.sec ||
				 (timeout.sec == now.sec && timeout.nsec < now.nsec))) {
			/* robomd2 watchdog */
			dcmDisableChannel(&DCMD1, 2);
			dcmDisableChannel(&DCMD1, 3);
		}

	}
}

