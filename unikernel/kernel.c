#include <core.h>
#include <core/exint_pass.h>
#include <core/process.h>
#include <core/thread.h>
#include <core/timer.h>

#define MSR_IA32_FS_BASE		0xC0000100

static int desc;
static tid_t tid;
#define LOCAL_TIMER_VECTOR 0xec
static int countint = 1000; // 1s
unsigned long long *heap;
static int ukls[32];

#define INFO(TEXT, ...) printf("[ukl-kernel]" TEXT, ##__VA_ARGS__)

// VMM内へのタイマ割り込み起点の処理
int vt_insert_timer_interrupt(void *data, int num) {
	if (num != LOCAL_TIMER_VECTOR)
		return num;
	if (countint <= 0) {
		INFO("shedule !\n");
		countint = 1000;
		schedule();
	} else {
		countint--;
	}
	return num;
}

static inline void
asm_wrmsr32 (ulong num, u32 a, u32 d)
{
	asm volatile ("wrmsr"
		      :
		      : "c" (num), "a" (a), "d" (d));
}

static inline void
asm_wrmsr64 (ulong num, u64 value)
{
	u32 a, d;

	a = (u32)value;
	d = (u32)(value >> 32);
	asm_wrmsr32 (num, a, d);
}

static int ukl_kernel_msghandler(int m, int c, struct msgbuf *buf, int bufcnt) {
	void *tmp1;
	phys_t tmp2;
	int heap_tmp;
	int r = -1;

	switch (c) {
	case 1:
		INFO("[ukl] init HEAP\n");
		break;
	case 2:
		INFO("ukl kernel: 2\n");
		// alloc_pages(&tmp1, &tmp2, 20);
		// printf("[ukcl] virt address: %08x (%lld), phys address: %08x (%lld)\n", (unsigned long long )tmp1, (unsigned long long )tmp1,
		//  (unsigned long long)tmp2, (unsigned long long)tmp2);
		// printf("[ukl] address(int): %d\n", (int)tmp1);
		INFO("[ukl] address(int): %d\n", (int)heap);
		*heap = 0;
		return (int)heap;
		break;
	case 3:
		INFO("ukl kernel: 3\n");
		heap = (unsigned long long *)buf->base;
		INFO("[ukl] heap address: %llx (%lld)",
			 (unsigned long long)heap, (unsigned long long)heap);
		break;
	case 4:
		INFO("get heap address (pid %d)\n", getpid());
		// heap_tmp = (int)msgsendint(getpid(), 1);
		// TODO
		// heap_tmp = (int)sys_msgsendint(0, 0, 0, 1, 1);
		heap_tmp = get_heap_start();
		INFO("heap addr: %x\n", heap_tmp);
		return heap_tmp;
	case 5:
		INFO("get container pid\n");
		return getpid();
	case 6: // TODO: delete おそらく不要
		INFO("set tls(base=%x)\n", 0);
		asm_wrmsr64 (MSR_IA32_FS_BASE, 0);
		break;
	default:
		break;
	}

	return 0;
}

static void timer_loop(void *handle, void *data) {
	// printf("ukl timer\n");
	timer_set(handle, 1000 * 1000 * 2);
	schedule();
}

static void new_uklprocess() {
	INFO("create ukl process\n");
	int d, d1;
	d = newprocess2("includeos");
	if (d < 0) {
		panic("new ukl process error\n");
	}
	d1 = msgopen ("ttyout");
	if (d1 < 0)
		panic ("msgopen ttyout");
	msgsenddesc (d, d1);
	msgsenddesc (d, d1);
	msgclose (d1);
	// start up
	msgsendint(d, 0);
	if (d < 0) {
		panic("start includeos process error");
	}
	INFO("PID %d: finish ukl process\n", d);
	return;
}

static void new_container() {
	int tid;
	INFO("create ukl thread\n");
	tid = thread_new(new_uklprocess, NULL, 0x1000 * 1024 * 1); // stack 4K * 1024 * 2
	// new_uklprocess();
	INFO("thread id: %d\n", tid);
	return;
}

static void
unikernel_init(void) {
	int d1, ukld;
	void *handle;

	// INFO("init ukl timer\n");
	// timer_set(handle, 1000 * 1000 * 11); // 11秒後

	// handle2 = timer_new(new_uklthread, NULL);
	// thread_new(new_uklthread, NULL, 4096 * 8);
	// schedule();
	// timer_set(handle2, 1000 * 1000 * 60);

	ukld = msgregister("ukl", ukl_kernel_msghandler);
	if (ukld < 0) {
		panic("cant register ukl\n");
	}
	INFO("registered ukl handler\n");

	INFO("new container 1\n");
	// new_container();
	// new_uklprocess();
	// handle = timer_new(new_container, NULL);
	// timer_set(handle, 1000 * 1000 * 0); // 10秒後
	// thread_new(new_uklprocess, NULL, 0x200000); // stack 2MB
	// thread_new(new_uklprocess, NULL, 0x1000 * 10); // stack 4K *10
	// new_uklprocess();
	
	new_container();

	// INFO("new container 2\n");
	// handle = timer_new(new_container, NULL);
	// timer_set(handle, 1000 * 1000 * 15); // 15秒後

	// INFO("new container 2\n");
	// new_container();
	// d = newprocess("hello");
	// d = newprocess("hello");
	// if (d < 0) {
	// panic("hello error");
	// }
	// msgsendint(d, 0);

	// printf("newprocess unikernel\n");
	// d = newprocess2("includeos");
	// if (d < 0) {
	// 	panic("new includeos process error");
	// }
	// printf("current pid is %d before start includeos\n", getpid());
	// printf("process d id %d\n", d);
	// msgsenddesc(d, ukld);
	// int dm = msgsendint(d, 0);
	// if (dm < 0) {
	// 	panic("start includeos process error");
	// }

	INFO("containers started \n");

	// if (d < 0)
	//     panic("panic new process unikernel");
	d1 = msgopen("ttyout");
	if (d1 < 0)
		panic("panic msgopen ttyout (unikernel)");

	// panic("success starting includeos\n");

}

INITFUNC("driver99", unikernel_init);
