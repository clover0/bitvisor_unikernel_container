#include <core/process.h>

static int desc;

static void out__b(unsigned short int port, unsigned char data)
{
	unsigned long flags;
	asm volatile("pushfq;"
				 "pop %0;"
				 : "=r"(flags));
	printf("eflags after in user out__b: %08lX\n", flags);
	asm volatile("outb %0,%1" ::"a"(data), "Nd"(port));
	printf("done outb\n");
}

static unsigned char in__b(unsigned short int port)
{
	unsigned char ret;
	asm volatile("inb %1,%0;"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

static int
unikernel_msghandler(int m, int c, struct msgbuf *buf, int bufcnt)
{
	unsigned short int select = 0x70;
	unsigned short int data = 0x71;
	unsigned char no_nmi = 0x80;
	unsigned char r_min = 0x2;
	unsigned char ret;

	unsigned short int cs;
	asm volatile("mov %%cs,%0"
				 : "=r"(cs));
	printf("CS register in unikerne lib: %d\n", cs);

	iopl(3);
	out__b(select, r_min | no_nmi);
	ret = in__b(data);

	printf("ret: %c, %08d\n", ret, ret);
	printf("finish handling ukl\n");
	return 0;
}

void unikernel_user_init()
{
	printf("init unikernel user\n");
	desc = msgregister("unikernel", unikernel_msghandler);
}