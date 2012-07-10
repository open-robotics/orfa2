
#include <stdio.h>
#include <ctype.h>

#include "eterm.h"

static eterm_node_t *rootnode = NULL;

/*
 * Utils
 */

static eterm_node_t *find_node(char command)
{
	eterm_node_t *it = rootnode;
	command = toupper((int)command);
	while (it) {
		if (it->command == command)
			return it;
		it = it->next;
	}
	return NULL;
}

/*
 * Help
 */

static bool_t help_cb(BaseSequentialStream *chp, char c, char *buf, bool_t reinit)
{
	eterm_node_t *it = rootnode;

	(void)buf;
	(void)reinit;
	if (c == '\n') {
		chprintf(chp, "Commands:\n");
		while (it) {
			chprintf(chp, "  '%c'\t\t%s\n",
				it->command, it->help);
			it = it->next;
		}
		return TRUE;
	}
	return FALSE;
}

static eterm_node_t help_nodes[] = {
	ETERM_INIT('?', "short help", FALSE, help_cb),
	ETERM_INIT('H', "short help", FALSE, help_cb)
};

/*
 * Public API
 */

void etermRegister(eterm_node_t *node)
{
	eterm_node_t *exist = find_node(node->command);
	if (exist)
		return;
	node->next = rootnode;
	rootnode = node;
}

extern void eterm_init_io_nodes(void);
extern void eterm_init_motor_nodes(void);
extern void eterm_init_ssc32_nodes(void);

void appEterm(BaseSequentialStream *chp, int argc, char *argv[])
{
	char buf[ETERM_BUFSZ];
	char *bufp=NULL;
	eterm_node_t *currnode = NULL;
	bool_t res;
	msg_t c;

	(void)argc;
	(void)argv;
	etermRegister(&help_nodes[0]);
	etermRegister(&help_nodes[1]);

	eterm_init_io_nodes();
	eterm_init_motor_nodes();
	eterm_init_ssc32_nodes();

	chprintf(chp, "ORFA ETerm 2.0\n\n");

	chprintf(chp, "Kernel:       %s\n", CH_KERNEL_VERSION);
	chprintf(chp, "Architecture: %s\n", CH_ARCHITECTURE_NAME);
#ifdef PLATFORM_NAME
	chprintf(chp, "Platform:     %s\n", PLATFORM_NAME);
#endif
#ifdef BOARD_NAME
	chprintf(chp, "Board:        %s\n", BOARD_NAME);
#endif
#if defined(__DATE__) && defined(__TIME__)
	chprintf(chp, "Build time:   %s - %s\n", __DATE__, __TIME__);
#endif

	chprintf(chp, "\n");
	help_cb(chp, '\n', buf, TRUE);

	while (!chThdShouldTerminate()) {
		c = chnGetTimeout((BaseChannel*)chp, MS2ST(100));
		if (c == Q_TIMEOUT)
			continue;

		if (c == '\r')
			c = '\n';

		if (!currnode && c == '\n')
			continue; /* blank line */

		if (!currnode) {
			currnode = find_node(c);
			if (!currnode) {
				chprintf(chp, "ERROR: unknown command '%c'.\n", c);
				continue;
			}
			if (currnode->buffered) {
				bufp = buf;
				*bufp++ = c;
			}
			res = currnode->cb(chp, c, buf, TRUE);
		}
		else {
			if (currnode->buffered) {
				if (c != '\n') {
					if (bufp < buf + sizeof(buf))
						*bufp++ = c;

					continue;
				}
				else {
					*bufp = '\0';
					res = currnode->cb(chp, c, buf, FALSE);
				}
			}
			else
				res = currnode->cb(chp, c, buf, FALSE);
		}

		if (res)
			currnode = NULL;
	}
}

