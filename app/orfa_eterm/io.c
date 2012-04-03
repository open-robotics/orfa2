
#include <string.h>

#include "eterm.h"

#define BUFSZ	32

static char buf[BUFSZ];
static char *p;
static bool_t adc_raw_out = FALSE;


static void pin_mode(BaseChannel *chp, char port, char pin, char mode)
{
	char *mode_str;

	switch (mode) {
		case 'A':
			mode_str = "ADC";
			break;

		case 'O':
			mode_str = "Out";
			break;

		case 'I':
		default:
			mode_str = "In";
			break;
	};

	pin = pin - '0';

	chprintf(chp, "PinMode%c%d=%s\n", port, pin, mode_str);
}

static void pin_get(BaseChannel *chp, char port, char pin)
{
	pin = pin - '0';

	chprintf(chp, "%c%d:%d\n", port, pin, 0);
}

static void pin_set(BaseChannel *chp, char port, char pin, char st)
{
	pin = pin - '0';
	st = st != '0';

	chprintf(chp, "%c%d=%d\n", port, pin, st);
}

static bool_t pin_adc_io_cb(BaseChannel *chp, char c, bool_t reinit)
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

	/* scan buffer */
	if (strncmp(buf, "PinMode", 7) == 0) {
		/* PinModeX0=Y */
		pin_mode(chp, buf[7], buf[8], buf[10]);
	}
	else if (strncmp(buf, "PinGet", 6) == 0) {
		/* PinGetX0 */
		pin_get(chp, buf[6], buf[7]);
	}
	else if (strncmp(buf, "PinSet", 6) == 0) {
		/* PinSetX0=Y */
		pin_set(chp, buf[6], buf[7], buf[9]);
	}
	else if (strncmp(buf, "AdcBits", 7) == 0) {
		/* compat */
		chprintf(chp, "AdcBits=12\n");
	}
	else if (strncmp(buf, "AdcRef", 6) == 0) {
		/* compat */
		chprintf(chp, "AdcRef=Int\n");
	}
	else if (strncmp(buf, "AdcRAW", 6) == 0) {
		adc_raw_out = buf[7] == '1';
		chprintf(chp, "AdcRAW=%d\n", adc_raw_out);
	}
	else {
		chprintf(chp, "ERROR: unknown io command \"%s\"\n", buf);
	}

	return TRUE;
}

static eterm_node_t io_nodes[] = {
	ETERM_INIT('P', "Pin control", pin_adc_io_cb),
	ETERM_INIT('A', "ADC control", pin_adc_io_cb)
};

void eterm_init_io_nodes(void)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(io_nodes); i++)
		etermRegister(&io_nodes[i]);
}

