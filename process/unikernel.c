#include <lib_printf.h>
#include <lib_syscalls.h>

#define HEAP_SIZE 1024*1024

int heap[HEAP_SIZE], heaplen = HEAP_SIZE;

int
_start (int a1, int a2)
{
    void unikernel_user_init();

    if (a1 != MSG_INT)
        exitprocess(1);
    printf("hello unikernel before user init\n");
    unikernel_user_init();
    printf("unikernel init!\n");
//    exitprocess (0);
    return 0;
}
