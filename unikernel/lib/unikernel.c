#include <core/process.h>


static int desc;

static int
unikernel_msgehandler(int m, int c, struct msgbuf *buf, int bufcnt)
{
    if (m != MSG_BUF)
        return -1;
    return -1;
}

void unikernel_user_init()
{
    desc = msgregister("unikernel", unikernel_msgehandler);
}