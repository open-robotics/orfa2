#ifndef ETERM_H
#define ETERM_H

#include "common.h"
#include "chprintf.h"

#define ETERM_INIT(_command, _help, _buffered, _callback) \
	{ _command, _help, _buffered, _callback, NULL }

#define ETERM_BUFSZ 32

typedef bool_t (*eterm_cb_t)(BaseSequentialStream *chp, char c, char *buf, bool_t reinit);

typedef struct eterm_node_s {
	char command;
	char *help;
	bool_t buffered;
	eterm_cb_t cb;
	struct eterm_node_s *next;
} eterm_node_t;

void etermRegister(eterm_node_t *parser);
void appEterm(BaseSequentialStream *chp, int argc, char *argv[]);

#endif /* ETERM_H */
