
#include <string.h>
#include <stdlib.h>

#include "eterm.h"

#define BUFSZ	32
static char buf[BUFSZ];
static char *p;

static bool_t motor_cb(BaseChannel *chp, char c, bool_t reinit)
{
	if (reinit) {
		p = buf;
		*p++ = c;
		return FALSE;
	}

	/* read to buffer */
	if (c != '\n') {
		if (p < buf + BUFSZ)
			*p++ = c;

		return FALSE;
	}
	p = '\0'; /* terminate buffer */

	if (strncmp(buf, "DrvLR=", 6) == 0) {
		int l, r;

		l = strtol(buf + 6, &p, 10);
		if (*p != ',')
			chprintf(chp, "ERROR: DrvLR=l,r\n");
		else {
			r = strtol(p + 1, NULL, 10);

			dcmEnableChannel(&DCMD1, 0, l * 10);
			dcmEnableChannel(&DCMD1, 1, r * 10);
		}
	}
	else {
		chprintf(chp, "ERROR: unknown drv command \"%s\"\n", buf);
	}

	return TRUE;
}

static eterm_node_t motor_node = ETERM_INIT('D', "DC Motor driver", motor_cb);

void eterm_init_motor_nodes(void)
{
	etermRegister(&motor_node);
}
