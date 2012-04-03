
#include <stdio.h>

#include "eterm.h"

static eterm_node_t *rootnode = NULL;

/*
 * Utils
 */

static eterm_node_t *find_node(char command)
{
	eterm_node_t *it = rootnode;
	command = toupper(command);
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

static bool_t help_cb(BaseChannel *chp, char c, bool_t reinit)
{
	eterm_node_t *it = rootnode;

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
	ETERM_INIT('?', "short help", help_cb),
	ETERM_INIT('H', "short help", help_cb)
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

void appEterm(BaseChannel *chp, int argc, char *argv[])
{
	eterm_node_t *currnode = NULL;
	bool_t res;
	char c;

	(void)argc;
	(void)argv;
	etermRegister(&help_nodes[0]);
	etermRegister(&help_nodes[1]);

	eterm_register_ssc32_nodes();

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
	help_cb(chp, '\n', TRUE);

	while (!chThdShouldTerminate()) {
		c = chIOGet(chp);

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
			res = currnode->cb(chp, c, TRUE);
		}
		else
			res = currnode->cb(chp, c, FALSE);

		if (res)
			currnode = NULL;
	}
}

