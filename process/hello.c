#include <lib_lineinput.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <lib_syscalls.h>

char hello[] = "hello!!";

int
_start (int a1, int a2)
{
    printf("%s\n", hello);
	exitprocess (0);
	return 0;
}
