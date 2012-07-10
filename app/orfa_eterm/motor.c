
#include <string.h>
#include <stdlib.h>

#include "eterm.h"

static bool_t motor_cb(BaseSequentialStream *chp, char c, char *buf, bool_t reinit)
{
	char *p;

	(void)c;
	if (reinit)
		return FALSE;

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
#if DCM_CHANNELS == 4
	else if (strncmp(buf, "DrvMdLR=", 8) == 0) {
		int l, r;

		l = strtol(buf + 8, &p, 10);
		if (*p != ',')
			chprintf(chp, "ERROR: DrvMdLR=l,r\n");
		else {
			r = strtol(p + 1, NULL, 10);

			dcmEnableChannel(&DCMD1, 2, l * 10);
			dcmEnableChannel(&DCMD1, 3, r * 10);
		}
	}
#endif /* DCM_CHANNELS == 4 */
	else {
		chprintf(chp, "ERROR: unknown drv command \"%s\"\n", buf);
	}

	return TRUE;
}

static eterm_node_t motor_node = ETERM_INIT('D', "DC Motor driver", TRUE, motor_cb);

void eterm_init_motor_nodes(void)
{
	etermRegister(&motor_node);
}
