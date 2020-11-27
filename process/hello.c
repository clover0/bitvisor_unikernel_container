#include <lib_lineinput.h>
#include <lib_printf.h>
#include <lib_string.h>
#include <lib_syscalls.h>
#include <lib_storage_io.h>

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

void out__b(unsigned short int port, unsigned char data)
{
	asm volatile("outb %0,%1" ::"a"(data), "Nd"(port));
}

void out__l(unsigned short int port, unsigned char data)
{
	asm volatile("out %0,%1" ::"a"(data), "Nd"(port));
}

unsigned char in__b(unsigned short int port)
{
	unsigned char ret;
	asm volatile("inb %1,%0;"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

unsigned char in__l(unsigned short int port)
{
	unsigned char ret;
	asm volatile("in %1,%0;"
				 : "=a"(ret)
				 : "Nd"(port));
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
	printf("ret: %c, %08d\n", ret, ret);
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

void tmp_callback(void *data, long long size)
{
	printf("hello tmp callback\n");
}

// void disk_io_example()
// {
// 	int retval, num, id, getsitzeret;
// 	void *data;

// 	retval = storage_io_init();
// 	printf("retval: %d\n", retval);
// 	id = 1;
// 	num = storage_io_get_num_devices(id);
// 	printf("storag get num devices: %d\n", num);

// 	getsitzeret = storage_io_aget_size(id, 8, tmp_callback, data);
// 	printf("storage_io_aget_size: %d\n", getsitzeret);
// }

void test_bv_read_write(){
	char *buf = "123456789";
	char recvbuf[100];
	unsigned long time = 0;
	bv_net_write(buf, 9);

	bv_net_read(recvbuf, 100);

	printf("recv buf %s\n", recvbuf);

	bv_get_time(&time);
	printf("time: %ld\n", time);
}

// mkudp


static void
wshort (char *off, unsigned short x)
{
	off[0] = (x >> 8);
	off[1] = x;
}

// for check
static int
mkudp (char *buf, char *src, int sport, char *dst, int dport,
       char *data, int datalen)
{
	short sum;

	// IPv4 ヘッダ
	/* TTL=64 */
	memcpy (buf, "\x45\x00\x00\x00\x00\x01\x00\x00\x40\x11\x00\x00", 12);
	wshort (buf + 2,  datalen + 8 + 20);
	memcpy (buf + 12, src, 4);
	memcpy (buf + 16, dst, 4);

	// UDPヘッダ
	wshort (buf + 20, sport);
	wshort (buf + 22, dport);
	wshort (buf + 24, datalen + 8);
	memcpy (buf + 26, "\x00\x11", 2); // check sum 
	memcpy (buf + 28, data, datalen); // UDP data

	sum = ~ipchecksum (buf + 12, datalen + 16);
	memcpy (buf + 26, &sum, 2);
	sum = ipchecksum (buf + 24, 4);
	memcpy (buf + 26, &sum, 2);
	sum = ipchecksum (buf, 20);
	memcpy (buf + 10, &sum, 2);
	return datalen + 8 + 20;
}

const int itr_count = 1000;
int test_io_netsend_peformance(){
	// Linux 上でも同一になりそうなコードがよい
	// UDP パケットをつくる
	unsigned int pktsiz = 0;
	unsigned long start_time,end_time = 0;
	char pkt[64 + 80 + 9];

	// char *pkt_p;
	char src_ip[4] = {192,168,0,196}; // 自宅PC
	// char src_ip[4] = {192,168,11,111}; // 研究室PC 右
	// char dst_ip[4] = {192,168,0,187}; // 自宅PC
	char dst_ip[4] = {192,168,0,181}; // 自宅PC USB-LAN
	// char dst_ip[4] = {192,168,11,4}; // 研究室PC USB-LAN

	int len = 0;
	char data[] = {'h','e', 'l', 'l', 'o'};

	len = 5; // 5 * 8 byte
	memcpy (pkt + 12, "\x08\x00", 2);
	pktsiz = mkudp (pkt + 14,
				(char *)src_ip, 514, // ip port
				(char *)dst_ip, 12049, // ip port
				data, len) + 14;

	bv_get_time(&start_time);
	for(int i=0; i< itr_count; i++){
		bv_net_write(pkt, pktsiz);
	}
	bv_get_time(&end_time);

	printf("result %ld / %d\n", end_time - start_time, itr_count);
	
	return 0;
}

int _start(int a1, int a2)
{
	printf("%s\n", hello);
	// test_bv_read_write();
	test_io_netsend_peformance();
	// syscall_example();
	// vmcall_example();
	// int_example();
	// iopl_example();
	// io_cmos_example();
	// disk_io_example();

	exitprocess(0);
	return 0;
}
