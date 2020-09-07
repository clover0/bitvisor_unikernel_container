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
void out__b(unsigned short int port, unsigned char data){
	unsigned long flags;
	asm volatile("pushfq;"
				 "pop %0;"
				 : "=r"(flags));
	printf("eflags after in user out__b: %08lX\n", flags);
	asm volatile("outb %0,%1" ::"a"(data), "Nd"(port));
	printf("done outb\n");
}

unsigned char in__b(unsigned short int port){
	unsigned char ret;
	asm volatile("inb %1,%0;":"=a"(ret):"Nd"(port));
	return ret;
}

void io_cmos_example()
{
	printf("io cmos start\n");
	// CMOSアクセス
	unsigned short int select = 0x70;
	unsigned short int data = 0x71;
	// unsigned short int data = 0x71;
	unsigned char no_nmi = 0x80;
	unsigned char r_min = 0x2;
	unsigned char ret;
	unsigned long flags;

	printf("iopl start\n");
	// unsigned short int* cs;

	asm volatile("pushfq;"
				 "pop %0;"
				 : "=r"(flags));
	printf("eflags in user: %08lX\n", flags);
	flags = 0;
	iopl(3);

	asm volatile("pushfq;"
				 "pop %0;"
				 : "=r"(flags));
	printf("eflags after in user io ex: %08lX\n", flags);

	// 別関数にするとIOPLがはずれてる
	out__b(select, r_min | no_nmi);
	ret = in__b(data);
	// asm volatile ("outb %0,%1" :: "a"(data), "Nd"(port));
	// asm volatile("inb %1,%0;":"=a"(ret):"Nd"(data));
	printf("ret: %c, %08d\n", ret,ret);
	printf("success I/O port \n");

	return;
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

void iopl_example()
{
	printf("iopl start\n");
	unsigned long flags;
	// unsigned short int* cs;

	asm volatile("pushfq;"
				 "pop %0;"
				 : "=r"(flags));
	printf("eflags in user: %08lX\n", flags);

	iopl(3);

	asm volatile("pushfq;"
				 "pop %0;"
				 : "=r"(flags));
	printf("eflags after in user: %08lX\n", flags);

}

int _start(int a1, int a2)
{
	printf("%s\n", hello);
	// syscall_example();
	// vmcall_example();
	// int_example();
	// iopl_example();
	io_cmos_example();
	exitprocess(0);
	return 0;
}
