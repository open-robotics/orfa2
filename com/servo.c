
#include "servo.h"
#include "chprintf.h"
#include <string.h>

#define ITERATION_STEP_MS 10
#define debug(...) chprintf((struct BaseChannel*)&SD1, __VA_ARGS__)

static VirtualTimer vtmr;

static rcswidth_t start_width[RCS_CHANNELS]; /* usec */
static rcswidth_t target_width[RCS_CHANNELS]; /* usec */
static systime_t total_time[RCS_CHANNELS]; /* msec */
static systime_t time_left[RCS_CHANNELS]; /* msec */
static bool_t endq_reported;

static void vtmr_func(void *p)
{
	bool_t updated = FALSE;
	signed tmp;

	for (unsigned i = 0; i < RCS_CHANNELS; i++)
		if (time_left[i] > 0) {
			updated = TRUE;
			endq_reported = FALSE;

			if (time_left[i] <= ITERATION_STEP_MS) {
				time_left[i] = 0;
				tmp = target_width[i];
			} else {
				time_left[i] -= ITERATION_STEP_MS;
				tmp = start_width[i];
				tmp -= target_width[i];
				tmp *= time_left[i];
				tmp /= total_time[i];
				tmp += target_width[i];
			}
			rcsEnableChannel(&RCSD1, i, tmp);
		}

	if (updated)
		rcsSync(&RCSD1);

	if (!updated && !endq_reported) {
		endq_reported = TRUE;
		/* TODO send event */
	}

	chVTSetI(&vtmr, MS2ST(ITERATION_STEP_MS), vtmr_func, p);
}

static size_t writet(void *ip, const uint8_t *bp, size_t n, systime_t time)
{
	size_t msg_cnt = 0, tr_n = 0;
	systime_t max_time = 0;
	servo_msg_t *data = (servo_msg_t *) bp;

	(void) ip;
	(void) time;
	if (n < sizeof(servo_msg_t))
		return 0;

	debug("servo_cmd->writet(ip, p, %u, t)\r\n", n);

	while (n >= sizeof(servo_msg_t)) {
		if (data->channel < RCS_CHANNELS && data->width != 0) {
			unsigned dx;
			unsigned pos = rcsGetWidth(&RCSD1, data->channel);

			if (data->width > pos)
				dx = data->width - pos;
			else
				dx = pos - data->width;

			if (data->speed != 0) {
				dx = (dx * 1000) / data->speed;
				if (dx < data->time)
					dx = data->time;
			}
			else
				dx = data->time;

			if (dx > max_time)
				max_time = dx;
		}

		tr_n += sizeof(servo_msg_t);
		n -= sizeof(servo_msg_t);
		msg_cnt++;
		data++;
	}

	if (max_time < ITERATION_STEP_MS)
		max_time = ITERATION_STEP_MS;

	for (data = (servo_msg_t *) bp;
			msg_cnt > 0; msg_cnt--, data++) {
		if (data->channel < RCS_CHANNELS && data->width != 0) {
			rcswidth_t pos = rcsGetWidth(&RCSD1, data->channel);

			/* TODO add mutex */
			start_width[data->channel] = pos;
			target_width[data->channel] = data->width;
			total_time[data->channel] = max_time;
			time_left[data->channel] = max_time;

			debug("servo[%d]: %d -> %d, %d ms\r\n", data->channel,
					pos, data->width, max_time);
		}
	}

	return tr_n;
}

static size_t readt(void *ip, uint8_t *bp, size_t n, systime_t time)
{
	(void) ip;
	(void) bp;
	(void) n;
	(void) time;

	return 0;
}

static ioflags_t getflags(void *ip)
{
	_ch_get_and_clear_flags_impl(ip);
}

static struct BaseAsynchronousChannelVMT vmt = {
	.writet = writet,
	.readt = readt,
	.getflags = getflags
};

/* public */

BaseAsynchronousChannel servo_cmd;

void servoInit()
{
	rcsStart(&RCSD1, &rcs_default_config);

	servo_cmd.vmt = &vmt;
	chEvtInit(&servo_cmd.event);
	servo_cmd.flags = IO_NO_ERROR;

	chSysLock();
	chVTSetI(&vtmr, MS2ST(ITERATION_STEP_MS), vtmr_func, &servo_cmd);
	chSysUnlock();
}
