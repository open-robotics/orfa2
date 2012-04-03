
#include "servo.h"

#define ITERATION_STEP_MS 20

bool_t servo_query_status;

/* Private */

static VirtualTimer vtmr;

static rcswidth_t start_width[RCS_CHANNELS]; /* usec */
static rcswidth_t target_width[RCS_CHANNELS]; /* usec */
static systime_t total_time[RCS_CHANNELS]; /* msec */
static systime_t time_left[RCS_CHANNELS]; /* msec */
static bool_t endq_reported;

static void vtmr_func(void *p)
{
	bool_t updated = FALSE;
	int tmp;

	for (size_t i = 0; i < RCS_CHANNELS; i++)
		if (time_left[i] > 0) {
			updated = TRUE;
			endq_reported = FALSE;
			servo_query_status = SERVO_QUERY_IN_PROGRESS;

			if (time_left[i] <= ITERATION_STEP_MS) {
				time_left[i] = 0;
				tmp = target_width[i];
			} else {
				time_left[i] -= ITERATION_STEP_MS;
				tmp = target_width[i] +
					  (((int)start_width[i] - (int)target_width[i])
					   * (int)time_left[i]
					   / (int)total_time[i]);
			}
			rcsEnableChannel(&RCSD1, i, tmp);
		}

	if (updated)
		rcsSync(&RCSD1);

	if (!updated && !endq_reported) {
		endq_reported = TRUE;
		servo_query_status = SERVO_QUERY_DONE;
		/* TODO send event */
	}

	chVTSetI(&vtmr, MS2ST(ITERATION_STEP_MS), vtmr_func, p);
}

/* Public API */

void servoCommand(servo_msg_t *msgs, size_t len)
{
	size_t i;
	systime_t max_time = 0;
	servo_msg_t *data;

	for (i = 0, data = msgs; i < len; i++, data++) {
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
	}

	if (max_time < ITERATION_STEP_MS)
		max_time = ITERATION_STEP_MS;

	for (i = 0, data = msgs; i < len; i++, data++) {
		if (data->channel < RCS_CHANNELS && data->width != 0) {
			rcswidth_t pos = rcsGetWidth(&RCSD1, data->channel);

			/* TODO add mutex */
			start_width[data->channel] = pos;
			target_width[data->channel] = data->width;
			total_time[data->channel] = max_time;
			time_left[data->channel] = max_time;
		}
	}
}

void servoInit(void)
{
	rcsStart(&RCSD1, &rcs_default_config);

	chSysLock();
	chVTSetI(&vtmr, MS2ST(ITERATION_STEP_MS), vtmr_func, NULL);
	chSysUnlock();
}

