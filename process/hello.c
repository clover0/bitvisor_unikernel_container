#include <lib_lineinput.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <lib_syscalls.h>

char hello[] = "hello!!";

void int_example()
{
	unsigned int ret = 5;
	__asm__ __volatile__("mov $1,  %%eax;" //システムコール番号
						 "int $0x80;"	   //80番で割り込みを発生させる
						 // "syscall;"
						 "mov %%eax, %0;" //返り値をpid変数にコピー
						 : "=r"(ret));
	printf("int example ret: %d\n", ret);
	return;
}

void io_cmos_example()
{
	// CMOSアクセス
	// unsigned short int port = 0x70;
	unsigned short int select = 0x70;
	unsigned short int data = 0x71;
	unsigned char no_nmi = 0x80;
	unsigned char r_min = 0x2;
	unsigned char ret;

	asm volatile("outl %0,%1" ::"a"(r_min | no_nmi), "Nd"(select));
	asm volatile("inb %1,%0"
				 : "=a"(ret)
				 : "Nd"(data));

	printf("success I/O port \n");
	printf("ret: %d \n", ret);

	// __asm__ __volatile__("inb %w1,%0"
	// 					 : "=a"(v)
	// 					 : "Nd"(port));
}

void vmcall_example()
{
	printf("call vmcall\n");
	unsigned int num = 8;
	__asm__ __volatile__(
		"movq %%rdx,%%rsi;"
		"movq %%rsi,%%rdi;"
		"movq $8, %%rdi;"
		"movl %0,%%eax;"
		"vmcall;"
		:
		: "g"(num));
	return;
}

void syscall_example()
{
	printf("call syscall\n");
	unsigned int num = 1;
	// __asm__ __volatile__("mov %eax,$0x08");
	// __asm__ __volatile__("syscall");
	__asm__ __volatile__(
		// "movq %%rdx,%%rsi;"
		// "movq %%rsi,%%rdi;"
		// "movq $1, %%rdi;"
		"movl %0,%%eax;"
		"syscall;"
		:
		: "g"(num));
	return;
}

int _start(int a1, int a2)
{
	printf("%s\n", hello);
	// syscall_example();
	// vmcall_example();
	// int_example();
	// io_cmos_example();
	iopl_example();
	exitprocess(0);
	return 0;
}

void iopl_example()
{
	printf("iopl start\n");
	iopl(3);
}
