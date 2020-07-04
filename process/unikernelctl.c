#include <lib_lineinput.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <lib_syscalls.h>


int
_start(int a1, int a2) {
    char buf[100];
    int d;

    d = msgopen("unikernel");
    if (d < 0) {
        printf("failed to open unikernelctl.\n");
        exitprocess(1);
    }
    for (;;) {
        printf("unikernelctl> ");
        lineinput(buf, 100);

        if (!strcmp(buf, "q"))
            break;
        if (!strcmp(buf, "exit"))
            break;
        if (!strcmp(buf, "list")) {
            msgsendint(d, 2);
            continue;
        }
        printf("command not found\n");
    }
    msgclose(d);
    exitprocess(0);
    return 0;
}
