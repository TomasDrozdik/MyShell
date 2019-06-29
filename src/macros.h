/* macros.h */

#ifndef MYSHELL_MACROS_H
#define MYSHELL_MACROS_H

#include <err.h>

#define MALLOC(size, value) if ((value =\
		malloc(size)) == NULL) {\
			err(1, "malloc");\
		};

#define SIGACTION(signum, act, oldact) if (\
			sigaction(signum, act, oldact) == -1) {\
		err(1, "waitpid");\
	};

#define WAITPID(pid, wstatus, options) if (\
			waitpid(pid, wstatus, options) == -1) {\
		err(1, "waitpid");\
	};

#define PIPE(pd) if (pipe(pd) == -1) {\
		err(1, "pipe");\
	};

#define CLOSE(fd) if (close(fd) == -1) {\
		err(1, "close");\
	};


#endif  /* MYSHELL_MACROS_H */
