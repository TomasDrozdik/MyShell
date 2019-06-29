/* caller.h */
/*
 * 	Library used to process and execute myshell commands
 * in form of struct expr as a output of bison
 */

#ifndef MYSHELL_SRC_CALLER_H_
#define MYSHELL_SRC_CALLER_H_

#include "parser_structs.h"

void
call(struct expr *expr);

#endif  /* MYSHELL_SRC_CALLER_H_ */
