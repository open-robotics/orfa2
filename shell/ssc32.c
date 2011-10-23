
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
		S_COMMIT,
	} state = S_UNK, last_state = S_UNK;
	servo_msg_t msgs[RCS_CHANNELS + 1];
	size_t msgs_len = 0;
	int accu = 0;
	bool_t time = FALSE;

	msgs[0].width = 0;
	msgs[0].channel = 255; // XXX

	while (TRUE) {
		c = chIOGet(chp);
		chIOPut(chp, c);
		if (c == '\03') // ^C
			return;

		switch (c) {
		case '#':
			state = S_CH;
			break;

		case 'P':
			state = S_POS;
			break;

		case 'S':
			state = S_SPEED;
			break;

		case 'T':
			state = S_TIME;
			time = TRUE;
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
				msgs[++msgs_len].channel = accu;
				break;

			case S_POS:
				msgs[msgs_len].width = accu;
				break;

			case S_SPEED:
				msgs[msgs_len].speed = accu;
				break;

			case S_TIME:
				chprintf(chp, "time: %d\n", accu);
				msgs[0].width = 0;
				msgs[0].channel = 255; // XXX
				msgs[0].speed = accu;
				break;
			}
			accu = 0;
			chprintf(chp, "msg[%d]: ch=%d pw=%d sp=%d\n",
					msgs_len,
					msgs[msgs_len].channel,
					msgs[msgs_len].width,
					msgs[msgs_len].speed);
		}

		if (state == S_COMMIT) {
			chprintf(chp, "commit\n");
			chIOWriteTimeout(&servo_cmd, (time)? msgs : &msgs[1],
					(time)? sizeof(servo_msg_t) * msgs_len + 1 : sizeof(servo_msg_t) * msgs_len,
					TIME_INFINITE);
			time = FALSE;
			accu = 0;
			msgs_len = 0;
			state = S_UNK;
		}

		last_state = state;
	}
}

