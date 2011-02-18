/*-
 * (c) 2009 by Author
 *
 * Author: Lukasz Podkalicki <lukasz@podkalicki.com>
 */

#include "misc.h"

jmp_buf jump;

typedef void (*sighandler_t)(int);

static void
abort_handler(void)
{

    longjmp(jump, 1);
}

void
install_abort_handler(void)
{
    sighandler_t error;

    error = signal(SIGABRT, (sighandler_t)abort_handler);
    if (error == SIG_ERR) {
        perror("signal");
        exit(-1);
    }
}

void
deinstall_abort_handler(void)
{
    sighandler_t error;

    error = signal(SIGABRT, SIG_DFL);
    if (error == SIG_ERR) {
        perror("signal");
        exit(-1);
    }
}
