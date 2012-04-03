#ifndef ETERM_H
#define ETERM_H

#include "common.h"
#include "chprintf.h"

#define ETERM_INIT(_command, _help, _callback) \
	{ _command, _help, _callback, NULL }

typedef bool_t (*eterm_cb_t)(BaseChannel *chp, char c, bool_t reinit);

typedef struct eterm_node_s {
	char command;
	char *help;
	eterm_cb_t cb;
	struct eterm_node_s *next;
} eterm_node_t;

void etermRegister(eterm_node_t *parser);
void appEterm(BaseChannel *chp, int argc, char *argv[]);

#endif /* ETERM_H */
