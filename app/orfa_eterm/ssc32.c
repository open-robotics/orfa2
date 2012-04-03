
#include <stdio.h>

#include "eterm.h"
#include "servo.h"

typedef enum {
	ST_GET_COMMAND,
	ST_ERROR,
	ST_PARSE_NUM
} mv_state_t;

static bool_t ssc32_move_cb(BaseChannel *chp, char c, bool_t reinit)
{
	static mv_state_t state = ST_GET_COMMAND;
	static servo_msg_t msgs[RCS_CHANNELS];
	static size_t msgs_len = 0;
	static size_t num = 0;
	static char cmd = ' ';

	if (reinit) {
		state = ST_GET_COMMAND;
		msgs_len = 0;
		cmd = ' ';
		num = 0;
	}

	c = toupper(c);

	switch (state) {
	case ST_PARSE_NUM:
		if (c >= '0' && c <= '9') {
			num = num * 10 + c - '0';
			state = ST_PARSE_NUM;
		}
		else {
			if (cmd == '#' && num < RCS_CHANNELS) {
				if (++msgs_len > RCS_CHANNELS)
					--msgs_len;
				msgs[msgs_len - 1] = (servo_msg_t){ .channel = num };
			}
			else if (msgs_len == 0) {
				/* catch # error */
			}
			else if (cmd == 'P') {
				msgs[msgs_len - 1].width = num;
			}
			else if (cmd == 'S') {
				msgs[msgs_len - 1].speed = num;
			}
			else if (cmd == 'T') {
				msgs[msgs_len - 1].time = num;
			}

			state = ST_GET_COMMAND;
		}

		if (state != ST_GET_COMMAND)
			break;

	case ST_GET_COMMAND:
		switch (c) {
		case '#':
		case 'P':
		case 'S':
		case 'T':
			cmd = c;
			num = 0;
			state = ST_PARSE_NUM;
			return FALSE;

		case ' ':
			return FALSE;

		case '\n':
			if (cmd != ' ')
				servoCommand(msgs, msgs_len);
			return TRUE;

		default:
			state = ST_ERROR;
			return FALSE;
		}
		break;

	default:
		state = ST_ERROR;
		break;
	}

	if (c != '\n')
		return FALSE;
	chprintf(chp, "ERROR: invalid # command");
	return TRUE;
}

static bool_t ssc32_query_cb(BaseChannel *chp, char c, bool_t reinit)
{
	static rcschannel_t channels[RCS_CHANNELS];
	static size_t chan_len = 0, num = 0;
	static bool_t error = FALSE;

	if (reinit) {
		chan_len = 0;
		error = FALSE;
		return FALSE;
	}

	if (error) {
		if (c == '\n') {
			chprintf(chp, "ERROR: invalid Q command\n");
			return TRUE;
		}
		return FALSE
	}

	if (chan_len == 0) {
		if (c == '\n') {
			chIOPut(chp,
					(servo_query_status == SERVO_QUERY_DONE)?
					'.' : '+');
			return TRUE;
		}
		else if (c == 'P') {
			++chan_len;
			num = 0;
			return FALSE;
		}

		if (c != ' ')
			error = TRUE;
	}
	else {
		if (c >= '0' && c <= '9') {
			num = num * 10 + c - '0';
			return FALSE;
		}
		else if (c == 'Q')
			return FALSE;
		else if (c == 'P') {
			channels[chan_len - 1] = num;
			if (++chan_len > RCS_CHANNELS)
				--chan_len;
			num = 0;
			return FALSE;
		}
		else if (c == '\n') {
			channels[chan_len - 1] = num;

			for (num = 0; num < chan_len; num++) {
				unsigned char r = rcsGetWidth(&RCSD1, channels[num]) / 10;
				chIOPut(chp, r);
			}

			return TRUE;
		}

		if (c != ' ')
			error = TRUE;
	}

	return c == '\n';
}

static eterm_node_t ssc32_nodes[] = {
	ETERM_INIT('#', "SSC-32 Servo Move / Group Move", ssc32_move_cb),
	ETERM_INIT('Q', "SSC-32 Query Status / Pulse Width", ssc32_query_cb)
};

void eterm_init_ssc32_nodes(void)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(ssc32_nodes); i++)
		etermRegister(&ssc32_nodes[i]);
}

