#include <core.h>
#include <core/process.h>

static int desc;

static void
unikernel_init(void) {
    int d, d1, uctld;

    d = newprocess("unikernel");
    if (d < 0)
        panic("new process unikernel");
    d1 = msgopen("ttyout");
    if (d1 < 0)
        panic("msgopen ttyout (unikernel)");

    msgsenddesc(d, d1);
    msgsenddesc(d, d1);
    msgclose(d1);
    msgsendint(d, 0);
    msgclose(d);

//    uctld = msgopen("unikernelctl");
//    if (uctld <0)
//        panic("msgopen unikernelctl");

    desc = msgopen("unikernel");
    if (desc < 0)
        panic("msgopen unikernel (unikernel)");
}

INITFUNC ("driver99", unikernel_init);
