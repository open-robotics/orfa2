
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
		S_COMMIT,
	} state = S_UNK, last_state = S_UNK;
	servo_msg_t msgs[RCS_CHANNELS + 1];
	size_t msgs_len = 0;
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
			state = S_POS;
			break;

		case 'S':
			state = S_SPEED;
			break;

		case 'T':
			state = S_TIME;
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
				msgs[++msgs_len].channel = accu;
				msgs[msgs_len].width = 0;
				msgs[msgs_len].speed = 0;
				msgs[msgs_len].time = 0;
				break;

			case S_POS:
				msgs[msgs_len].width = accu;
				break;

			case S_SPEED:
				msgs[msgs_len].speed = accu;
				break;

			case S_TIME:
				msgs[msgs_len].time = accu;
				break;

			default:
				break;
			}
			accu = 0;
		}

		if (state == S_COMMIT) {
			chprintf(chp, "commit\n");
			for (size_t i=1; i < msgs_len + 1; i++) {
				chprintf(chp, "msg[%d]: ch=%d pw=%d sp=%d tm=%d\n",
						i,
						msgs[i].channel, msgs[i].width,
						msgs[i].speed, msgs[i].time
						);
			}

			chIOWriteTimeout(&servo_cmd, (const uint8_t *)&msgs[1],
					sizeof(servo_msg_t) * msgs_len, TIME_INFINITE);

			accu = 0;
			msgs_len = 0;
			state = S_UNK;
		}

		last_state = state;
	}
}

