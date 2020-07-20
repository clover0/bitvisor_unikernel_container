#include <lib_lineinput.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <lib_syscalls.h>

char hello[] = "hello!!";

int _start(int a1, int a2)
{
	printf("%s\n", hello);
	unsigned char v = 'a';
	unsigned short int port = 0x70;
	__asm__ __volatile__("inb %w1,%0"
						 : "=a"(v)
						 : "Nd"(port));
	exitprocess(0);
	return 0;
}
