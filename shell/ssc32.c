
#include <string.h>
#include <stdlib.h>

#include "ssc32.h"
#include "chprintf.h"

void shSsc32Sync(BaseChannel *chp, int argc, char *argv[])
{
	char c;
	enum {
		S_UNK,
		S_CH,
		S_POS,
		S_SPEED,
		S_TIME,
		S_QUERY,
		S_QUERY_POS,
		S_COMMIT,
	} state = S_UNK, last_state = S_UNK;
	enum {
		M_MOVE,
		M_QUERY
	} mode = M_MOVE;
	union {
		servo_msg_t msgs[RCS_CHANNELS + 1];
		rcschannel_t query_ch[RCS_CHANNELS + 1];
	} d;
	size_t i, msgs_len = 0;
	int accu = 0;

	while (!chThdShouldTerminate()) {
		c = chIOGet(chp);
		/*chIOPut(chp, c);*/
		if (c == '\03') // ^C
			return;

		c = toupper(c);

		switch (c) {
		case '#':
			state = S_CH;
			break;

		case 'P':
			if (mode == M_MOVE)
				state = S_POS;
			else
				state = S_QUERY_POS;
			break;

		case 'S':
			state = S_SPEED;
			break;

		case 'T':
			state = S_TIME;
			break;

		case 'Q':
			state = S_QUERY;
			mode = M_QUERY;
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			accu = (accu * 10) + (c - '0');
			break;

		case ' ':
		case '\t':
			break;

		case '\r':
		case '\n':
			state = S_COMMIT;
			break;

		default:
			state = S_UNK;
			break;
		}

		if (last_state != state && state != S_UNK) {
			switch (last_state) {
			case S_CH:
				d.msgs[++msgs_len].channel = accu;
				d.msgs[msgs_len].width = 0;
				d.msgs[msgs_len].speed = 0;
				d.msgs[msgs_len].time = 0;
				break;

			case S_POS:
				d.msgs[msgs_len].width = accu;
				break;

			case S_SPEED:
				d.msgs[msgs_len].speed = accu;
				break;

			case S_TIME:
				d.msgs[msgs_len].time = accu;
				break;

			case S_QUERY_POS:
				d.query_ch[++msgs_len] = accu;
				break;

			default:
				break;
			}
			accu = 0;
		}

		if (state == S_COMMIT) {
			if (mode == M_MOVE) {
				chIOWriteTimeout(&servo_cmd, (const uint8_t *)&d.msgs[1],
						sizeof(servo_msg_t) * msgs_len, TIME_INFINITE);
			}
			else { /* mode == M_QUERY */
				if (msgs_len) { /* QP<ch> */
					for (i = 1; i <= msgs_len; i++)
						chIOPut(chp, (d.query_ch[i] < RCS_CHANNELS)?
								rcsGetWidth(&RCSD1, d.query_ch[i]) / 10 : 0);
				}
				else
					chIOPut(chp, (servo_query_status == SERVO_QUERY_DONE)?
							'.' : '+');
			}

			accu = 0;
			msgs_len = 0;
			state = S_UNK;
			mode = M_MOVE;
		}

		last_state = state;
	}
}

