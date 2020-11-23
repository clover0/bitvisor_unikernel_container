#include <core/mm.h>
#include <core/process.h>
#include <core/initfunc.h>
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "tcpip.h"

static int desc;
static int destport;
static struct tcp_pcb *httpfs_pcb;
// Hostヘッダがないと接続できないので注意
// static char *send_buf = "GET /create/file10 HTTP/1.1\r\nHost: 192.168.0.187\r\n\r\n ";
static char *send_buf = "GET /read/file2?offset=0&size=200 HTTP/1.1\r\nHost: 192.168.0.187\r\n\r\n ";


struct arg {
	int ipaddr_a[4];
	int port;
};
static ip_addr_t destip;

static err_t
httpfs_sent (void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	printf ("Sent.\n");
	return ERR_OK;
}

static err_t
httpfs_recv (void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	int i;
	char *str;

	printf ("Received.\n");
	if (!p) {
		/* Disconnected by remote. */
		printf ("Disconnected!\n");
		httpfs_pcb = NULL;
		return ERR_OK;
	} else if (err != ERR_OK) {
		/* Error occurred. */
		printf ("Error: %d\n", err);
		return err;
	} else {
		/* Really received. */
		tcp_recved (pcb, p->len);
		str = p->payload;
		for (i = 0; i < p->len; i++)
			printf ("%c", str[i]);
		pbuf_free(p);
		return ERR_OK;
	}
}

static void
httpfs_error (void *arg, err_t err)
{
	printf ("Error: %d\n", err);
}

static err_t
httpfs_connected (void *arg, struct tcp_pcb *pcb, err_t err)
{
	if (err == ERR_OK) {
		printf ("HTTPfs Connection established!\n");
		tcp_arg (pcb, NULL);
		tcp_sent (pcb, httpfs_sent);
		tcp_recv (pcb, httpfs_recv);
		tcp_err (pcb, httpfs_error);
	} else {
		printf ("Connection missed!\n");
	}
	return ERR_OK;
}

static void
httpfs_init (int *ipaddr, int port)
{
	struct tcp_pcb *pcb;
	err_t e;

	IP4_ADDR (&destip,
		  ipaddr[0],
		  ipaddr[1],
		  ipaddr[2],
		  ipaddr[3]);
	destport = port;

	printf ("New Connection.\n");
	pcb = tcp_new ();
	if (pcb) {
		e = tcp_connect (pcb, &destip, destport,
				 httpfs_connected);
		if (e == ERR_OK) {
			printf ("Connecting...\n");
			httpfs_pcb = pcb;
		} else {
			printf ("Connect failed!\n");
		}
	} else {
		printf ("New context failed.\n");
	}
}


static int
send (void)
{
	err_t err;
	struct tcp_pcb *pcb = httpfs_pcb;
	int buflen;

	printf("do send\n");
	if (!httpfs_pcb) {
		printf ("No connection.\n");
		return -1;
	} else {
		printf ("Connection found.\n");
	}

	buflen = tcp_sndbuf (pcb);
	if (buflen >= strlen (send_buf)) {
		printf ("Space available: %d\n", buflen);
		err = tcp_write (pcb, send_buf, strlen (send_buf),
				 TCP_WRITE_FLAG_COPY);
		if (err == ERR_OK) {
			printf ("Enqueue succeeded.\n");
			err = tcp_output (pcb);
			if (err == ERR_OK) {
				printf ("Send succeeded.\n");
			} else {
				printf ("Send failed.\n");
				return -2;
			}
		} else {
			printf ("Enqueue failed.\n");
			return -3;
		}
	} else {
		printf ("Space unavailable: %d\n", buflen);
		return -4;
	}
	return 0;
}

static void
httpfs_send (void *arg)
{
	send ();
}

static void
httpfs_connect (void *arg)
{
	struct arg *a = arg;

	httpfs_init (a->ipaddr_a, a->port);
	free (a);
}

static void
httpfs_connect_and_send(void *arg)
{
	// httpのパケットが連続で正常に送れないので
	// 接続をセットで行う
	// たぶん非効率
	httpfs_connect(arg);
	httpfs_send(arg);
	// tcp_close(httpfs_pcb);
}

static int
container_msghandler(int m, int c, struct msgbuf *buf, int bufcnt) {
	unsigned char ret = 0;
	struct arg *a;

	printf("receive m:%d c:%d in ip container\n", m, c);
	switch (c) {
	case 2:
		printf("hello 2\n");
		break;
	case 3: // connect
		printf("connect server\n");
		a = alloc(sizeof *a);
		if (a) {
			// 192.168.0.187:12049
			a->ipaddr_a[0] = 0xC0;
			a->ipaddr_a[1] = 0xA8;
			a->ipaddr_a[2] = 0x00;
			a->ipaddr_a[3] = 0xBB;
			a->port = 12049;

			printf("begin tcp\n");
			tcpip_begin(httpfs_connect, a);
			ret = 0;
		} else {
			ret = -1;
		}
		break;
	case 4: // send request
		printf("send tcp\n");
		tcpip_begin(httpfs_send, NULL);
		ret = 0;
		break;
	case 5: // connect and send
		a = alloc(sizeof *a);
		if (a) {
			// 192.168.0.187:12049
			a->ipaddr_a[0] = 0xC0;
			a->ipaddr_a[1] = 0xA8;
			a->ipaddr_a[2] = 0x00;
			a->ipaddr_a[3] = 0xBB;
			a->port = 12049;

			printf("begin tcp\n");
			tcpip_begin(httpfs_connect_and_send, a);
			ret = 0;
		} else {
			ret = -1;
		}
	default:
		break;
	}

	printf("finish handling\n");
	return ret;
}

static void 
container_ctl_init(void)
{
	printf("init container ctl\n");
	desc = msgregister("container", container_msghandler);
}

INITFUNC ("msg0", container_ctl_init);
