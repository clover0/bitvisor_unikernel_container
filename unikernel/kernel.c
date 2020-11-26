#include <core.h>
#include <core/process.h>
#include <core/timer.h>
#include <core/thread.h>
#include <core/exint_pass.h>

static int d, desc;
static tid_t tid;
#define LOCAL_TIMER_VECTOR 0xec
static int countint = 1000; // 1s

// VMM内へのタイマ割り込み起点の処理
int
vt_insert_timer_interrupt(void *data, int num)
{
	if (num != LOCAL_TIMER_VECTOR)
		return num;
	if (countint <= 0)
	{
		printf("shedule !\n");
		countint = 1000;
		schedule();
	}
	else
	{
		countint--;
	}
	return num;
}

static int ukl_kernel_msghandler(int m, int c, struct msgbuf *buf, int bufcnt){
    switch (c)
    {
    case 2:
        printf("ukl kernel: hello2\n");
        break;
    default:
        break;
    }
    return 0;
}

static void timer_loop(void *handle, void *data)
{
    // printf("ukl timer\n");
    timer_set(handle, 1000 * 1000 * 2);
    schedule();
}

static void new_uklprocess()
{   
    printf("create ukl process\n");
    d = newprocess("unikernel");
    if (d < 0 ){
        printf("new ukl process error");
    }
    msgsendint (d, 0);
    printf("PID %d: finish ukl process\n", d);
    return;
}

static void new_uklthread()
{
    printf("create ukl thread\n");
    // tid = thread_new(newprocess, "unikernel", 4096 * 8);
    tid = thread_new(new_uklprocess, NULL, 4096 * 8);
    printf("ukl tid: %d\n", tid);
}

static void
unikernel_init(void)
{
    int d1, ukld;
    void *handle, *handle2;

    printf("init ukl timer\n");
    // handle = timer_new(timer_loop, NULL);
    // timer_set(handle, 1000 * 1000 * 11); // 11秒後

    handle2 = timer_new(new_uklthread, NULL);
    thread_new(new_uklthread, NULL, 4096 * 8);
    // schedule();
    // timer_set(handle2, 1000 * 1000 * 60);


    printf("newprocess unikernle\n");
    // if (d < 0)
    //     panic("panic new process unikernel");
    d1 = msgopen("ttyout");
    if (d1 < 0)
        panic("panic msgopen ttyout (unikernel)");
    
    // for logging
    ukld = msgregister("ukl", ukl_kernel_msghandler);
    if (ukld < 0) {
        panic("cant register ukl\n");
    }
    printf("registered ukl handler\n");
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
