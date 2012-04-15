
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


EvTimer servo_et, din_et;
uint8_t pin_mode[PALMAP_PADS_SIZE];

/* orfa2_msgs::SetupChannel::Request::CONST */
#define INPUT		0
#define OUTPUT		1
#define ANALOG_IN	2

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

	/* TODO watchdog using Header stamp */
	pw0 = cmd_msg.speed[0];
	pw1 = cmd_msg.speed[1];

	dcmEnableChannel(&DCMD1, 0, pw0);
	dcmEnableChannel(&DCMD1, 1, pw1);
}

void motor_md2_cb(const orfa2_msgs::Motor & cmd_msg)
{
	dcmwidth_t pw2, pw3;

	/* TODO watchdog using Header stamp */
	pw2 = cmd_msg.speed[0];
	pw3 = cmd_msg.speed[1];

	dcmEnableChannel(&DCMD1, 2, pw2);
	dcmEnableChannel(&DCMD1, 3, pw3);
}

void stop_cb(const roscpp::Empty::Request & req, roscpp::Empty::Response & res)
{
	nh.logerror("~stop not implemented");
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

void appRos(BaseChannel *chp, int argc, char *argv[])
{
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
	}
}

