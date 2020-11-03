#include <core/process.h>
#include <core/thread.h>
#include <core/time.h>

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

inline void out__l(unsigned short port, unsigned int data)
{
	asm volatile("outl %0,%1" ::"a"(data), "d"(port));
}

static unsigned char in__b(unsigned short int port)
{
	unsigned char ret;
	asm volatile("inb %1,%0;"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

inline unsigned int in__l(unsigned short port)
{
	unsigned int ret;
	asm volatile("inl %1,%0;"
				 : "=a"(ret)
				 : "d"(port));
	return ret;
}

unsigned short PCIConfigReadWord(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset)
{
	unsigned int address;
	unsigned int lbus = (unsigned int)bus;
	unsigned int lslot = (unsigned int)slot;
	unsigned int lfunc = (unsigned int)func;
	unsigned short tmp = 0;

	// コンフィギュレーションアドレスを作成
	address = (unsigned int)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((unsigned int)0x80000000));

	// アドレスの書き出し
	out__l(0xCF8, address);
	// データの読み込み
	tmp = (unsigned short)((in__l(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
	return (tmp);
}

unsigned short PCICheckVendor(unsigned char bus, unsigned char slot)
{
	unsigned short vendor, device;
	// ベンダーなし(0xFFFF)の場合、デバイスは存在しないことになる
	if ((vendor = PCIConfigReadWord(bus, slot, 0, 0)) != 0xFFFF)
	{
		device = PCIConfigReadWord(bus, slot, 0, 2);
	}
	return (vendor);
}

void current_eflags()
{
	unsigned long flags;
	asm volatile("pushfq;"
				 "pop %0;"
				 : "=r"(flags));
	printf("eflags in user: %08lX\n", flags);

	return;
}

void current_priv_level()
{
	unsigned short int cs;
	asm volatile("mov %%cs,%0"
				 : "=r"(cs));
	printf("CS register in unikerne lib: %x\n", cs);
	printf("CPL: %x\n", cs & 3);
}

static int
unikernel_msghandler(int m, int c, struct msgbuf *buf, int bufcnt)
{
	unsigned short int select = 0x70;
	unsigned short int data = 0x71;
	unsigned char no_nmi = 0x80;
	unsigned char r_min = 0x2;
	unsigned char ret;
	int id = 0;

	unsigned short int cs;

	printf("receive m:%d c:%d in lib ukl\n", m, c);
	switch (c)
	{
	case 1:
		asm volatile("mov %%cs,%0"
					 : "=r"(cs));
		current_priv_level();
		current_eflags();

		iopl(3);
		out__b(select, r_min | no_nmi);
		ret = in__b(data);
		// printf("cmos min: %d\n", ret);
		printf("cmos min x: %x\n", ret);

		// check virtio device
		// id = PCICheckVendor(2, 0); // pro1000
		// id = PCICheckVendor(0, 1); // virtual device
		id = PCICheckVendor(3, 0);
		// virtio(redhat)であるはず -> 素のNICが見えてしまった
		printf("vendor %x\n", id);
		break;
	case 2:
		printf("hello 2\n");
		break;
	default:
		break;
	}

	printf("finish handling ukl\n");
	return 0;
}

void unikernel_user_init()
{
	int tid;
	printf("init unikernel user\n");
	desc = msgregister("unikernel", unikernel_msghandler);
}