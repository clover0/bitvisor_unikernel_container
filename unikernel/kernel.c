#include <core.h>
#include <core/exint_pass.h>
#include <core/process.h>
#include <core/thread.h>
#include <core/timer.h>

static int desc;
static tid_t tid;
#define LOCAL_TIMER_VECTOR 0xec
static int countint = 1000; // 1s
unsigned long long *heap;
static int ukls[32];

#define INFO(TEXT, ...) printf("[ukl-kernel]" TEXT , ##__VA_ARGS__)

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

static int ukl_kernel_msghandler(int m, int c, struct msgbuf *buf, int bufcnt) {
	void *tmp1;
	phys_t tmp2;
	int heap_tmp;

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
		INFO("[ukl]get heap address (pid %d)\n", getpid());
		// heap_tmp = (int)msgsendint(getpid(), 1);
		// TODO
		// heap_tmp = (int)sys_msgsendint(0, 0, 0, 1, 1);
		heap_tmp = get_heap_start();
		INFO("[ukl]heap addr: %x\n", heap_tmp);
		return heap_tmp;
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
	int d = newprocess2("includeos");
	if (d < 0) {
		INFO("new ukl process error\n");
		panic("new ukl process error\n");
	}
	msgsendint(d, 0);
	if (d < 0) {
		panic("start includeos process error");
	}
	INFO("PID %d: finish ukl process\n", d);
	return;
}

static void new_container() {
	INFO("create ukl thread\n");
	// tid = thread_new(newprocess, "unikernel", 4096 * 8);
	tid = thread_new(new_uklprocess, NULL, 0x1000 * 10); // 4K *2
	INFO("ukl tid: %d\n", tid);
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
	handle = timer_new(new_container, NULL);
	timer_set(handle, 1000 * 1000 * 10); // 10秒後

	// new_container();
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

	// msgsenddesc(d, d1);
	// msgsenddesc(d, d1);
	// msgclose(d1);
	// msgsendint(d, 0); // start process/unikernel.bin
	// msgclose(d);

	// desc = msgopen("unikernel");
	// if (desc < 0)
	//     panic("panic msgopen unikernel (unikernel)");
}

INITFUNC("driver99", unikernel_init);
