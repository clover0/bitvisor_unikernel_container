#include <core/process.h>


static int desc;

static int
unikernel_msghandler(int m, int c, struct msgbuf *buf, int bufcnt)
{
    // if (m != MSG_BUF)
        // return -1;
    printf("handle ukl");
    return 0;
}

void unikernel_user_init()
{
    printf("init unikernel user");
    desc = msgregister("unikernel", unikernel_msghandler);
}