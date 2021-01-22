#include <core.h>
#include <core/exint_pass.h>
#include <core/process.h>
#include <core/thread.h>
#include <core/timer.h>

#define INFO(TEXT, ...) printf("[ukl-kernel]" TEXT, ##__VA_ARGS__)

static int ukl_kernel_msghandler(int m, int c, struct msgbuf *buf, int bufcnt) {
	int r = 0;

	switch (c) {
	case 5:
		INFO("get container pid\n");
		r = getpid();
		break;
	default:
		break;
	}

	return r;
}

static void new_uklprocess() {
	INFO("create ukl process\n");
	INFO("started at %lld\n", get_time()); // get_time=micro sec
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
	INFO("thread id: %d\n", tid);
	return;
}

static void
unikernel_init(void) {
	int d1, ukld;

	ukld = msgregister("ukl", ukl_kernel_msghandler);
	if (ukld < 0)
		panic("cant register ukl\n");
	INFO("registered ukl handler\n");

	INFO("new container\n");
	new_container();
	new_container();
	new_container();
	new_container();
	new_container();//5
	new_container();
	new_container();
	new_container();
	new_container();
	new_container();//10
	INFO("containers started \n");

	d1 = msgopen("ttyout");
	if (d1 < 0)
		panic("panic msgopen ttyout (unikernel)");
}

INITFUNC("driver99", unikernel_init);
