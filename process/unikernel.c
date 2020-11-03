#include <lib_printf.h>
#include <lib_syscalls.h>

#define HEAP_SIZE 1024 * 1024

int heap[HEAP_SIZE], heaplen = HEAP_SIZE;

int _start(int a1, int a2)
{
    int ukld;
    long long int i = 1000000;
    struct msgbuf mbuf;
    char buf[65536];
    void unikernel_user_init();

    if (a1 != MSG_INT)
        exitprocess(1);

    unikernel_user_init();

    ukld = msgopen("ukl");
    if (ukld >= 0)
    {
        msgsendint(ukld, 2);
    }
    else
    {
        printf("cant open unikernel\n");
        exitprocess(1);
    }

    printf("unikernel start!\n");
    // for (;;)
    // {
    //     if (i <= 0)
    //     {
    //         msgsendint(ukld, 2);
    //         i = 1000000;
    //     }
    //     else
    //         i--;
    //     bv_yield();
    // }
    exitprocess(1);
    return 0;
}
