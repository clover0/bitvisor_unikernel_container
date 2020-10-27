#include <lib_printf.h>
#include <lib_syscalls.h>

#define HEAP_SIZE 1024 * 1024

int heap[HEAP_SIZE], heaplen = HEAP_SIZE;

void unikernel_thread()
{
    unsigned int time = 0, m0;
    // m0 = msgopen("time");
    // msgsendint(m0, 0);
    for (;;)
    {
        if (time % 1000 == 0)
        {
            printf("ukltd!");
        }
        time++;
    }
}

int _start(int a1, int a2)
{
    void unikernel_user_init();

    if (a1 != MSG_INT)
        exitprocess(1);
    printf("hello unikernel before user init\n");
    unikernel_user_init();
    printf("unikernel init!\n");
    // unikernel_thread();
    //    exitprocess (0);
    return 0;
}
