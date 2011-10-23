
#include "servo.h"
#include "chprintf.h"
#include <string.h>

#define ITERATION_STEP 10
#define debug(...) chprintf((struct BaseChannel*)&SD1, __VA_ARGS__)

static VirtualTimer vtmr;

static rcswidth_t start_width[RCS_CHANNELS];
static rcswidth_t target_width[RCS_CHANNELS];
static systime_t total_time[RCS_CHANNELS];
static systime_t time_left[RCS_CHANNELS];
static bool_t endq_reported;

static void vtmr_func(void *p)
{
	bool_t updated = FALSE;
	signed tmp;

	for (unsigned i = 0; i < RCS_CHANNELS; i++)
		if (time_left[i] > 0) {
			updated = TRUE;
			endq_reported = FALSE;

			if (time_left[i] <= ITERATION_STEP) {
				time_left[i] = 0;
				tmp = target_width[i];
			} else {
				time_left[i] -= ITERATION_STEP;
				tmp = start_width[i];
				tmp -= target_width[i];
				tmp *= time_left[i];
				tmp /= total_time[i];
				tmp += target_width[i];
			}
			rcsEnableChannel(&RCSD2, i, tmp);
		}

	if (updated)
		rcsSync(&RCSD2);

	if (!updated && !endq_reported) {
		endq_reported = TRUE;
		// TODO send event
	}

	chVTSetI(&vtmr, MS2ST(ITERATION_STEP), vtmr_func, p);
}

static size_t writet(void *ip, uint8_t *bp, size_t n, systime_t time)
{
	// TODO: simplyfiy it

	rcswidth_t target[RCS_CHANNELS];
	unsigned max_speed[RCS_CHANNELS];
	systime_t max_time = 0;
	servo_msg_t *data = (servo_msg_t *)bp;
	size_t tr_n = 0;

	if (n < sizeof(servo_msg_t)) {
		return 0;
	}

	debug("servo writet(ip, bp, %d, t)\r\n", n);

	memset(&target, 0, sizeof(target));
	memset(&max_speed, 0, sizeof(max_speed));

	while (n >= sizeof(servo_msg_t)) {
		if (data->channel == SERVO_TIME_CH)
			max_time = data->speed;
		else if (data->channel < RCS_CHANNELS) {
			target[data->channel] = data->width;
			max_speed[data->channel] = data->speed;
		}
		tr_n += sizeof(servo_msg_t);
		n -= sizeof(servo_msg_t);
		data++;
	}

	debug("tr_n %d\r\n", tr_n);

	for (unsigned i = 0; i < RCS_CHANNELS; i++)
		if (target[i] != 0 && max_speed[i] != 0) {
			unsigned dx = 0;
			unsigned pos = RCSD2.widths[i]; // XXX
			if (target[i] > pos)
				dx = target[i] - pos;
			else
				dx = pos - target[i];
			dx = (dx * 1000) / max_speed[i];
			if (dx > max_time)
				max_time = dx;
		}

	if (max_time < ITERATION_STEP)
		max_time = ITERATION_STEP;

	debug("time2go %d\r\n", max_time);

	for (unsigned i = 0; i < RCS_CHANNELS; i++)
		if (target[i] != 0) {
			unsigned pos = RCSD2.widths[i]; // XXX
			debug("s[%d]: %d -> %d\r\n", i, pos, target_width[i]);
			start_width[i] = pos;
			target_width[i] = target[i];
			total_time[i] = max_time;
			time_left[i] = max_time;
		}

	return tr_n;
}

static size_t readt(void *ip, uint8_t *bp, size_t n, systime_t time)
{
	return 0;
}

static ioflags_t getflags(void *ip)
{
	_ch_get_and_clear_flags_impl(ip);
}

static struct BaseAsynchronousChannelVMT vmt = {
	.write = NULL,
	.read = NULL,
	.putwouldblock = NULL,
	.getwouldblock = NULL,
	.put = NULL,
	.get = NULL,
	.writet = writet,
	.readt = readt,
	.getflags = getflags
};

/* public */

BaseAsynchronousChannel servo_cmd;

static RCServoConfig rcscfg = {
	{
	RCS_CHANNEL(GPIOE, GPIOE_S00),
	RCS_CHANNEL(GPIOE, GPIOE_S01),
	RCS_CHANNEL(GPIOE, GPIOE_S02),
	RCS_CHANNEL(GPIOE, GPIOE_S03),
	RCS_CHANNEL(GPIOE, GPIOE_S04),
	RCS_CHANNEL(GPIOE, GPIOE_S05),
	RCS_CHANNEL(GPIOE, GPIOE_S06),
	RCS_CHANNEL(GPIOD, GPIOD_S07),
	RCS_CHANNEL(GPIOD, GPIOD_S08),
	RCS_CHANNEL(GPIOD, GPIOD_S09),
	RCS_CHANNEL(GPIOD, GPIOD_S10),
	RCS_CHANNEL(GPIOD, GPIOD_S11),
	RCS_CHANNEL(GPIOC, GPIOC_S12),
	RCS_CHANNEL(GPIOC, GPIOC_S13),
	RCS_CHANNEL(GPIOC, GPIOC_S14),
	RCS_CHANNEL(GPIOC, GPIOC_S15),
	RCS_CHANNEL(GPIOA, GPIOA_S16),
	RCS_CHANNEL(GPIOC, GPIOC_S17),
	RCS_CHANNEL(GPIOC, GPIOC_S18),
	RCS_CHANNEL(GPIOC, GPIOC_S19)
	}
};

void servoInit()
{
	rcsStart(&RCSD2, &rcscfg);

	servo_cmd.vmt = &vmt;
	chEvtInit(&servo_cmd.event);
	servo_cmd.flags = IO_NO_ERROR;

	chSysLock();
	chVTSetI(&vtmr, MS2ST(ITERATION_STEP), vtmr_func, &servo_cmd);
	chSysUnlock();
}
