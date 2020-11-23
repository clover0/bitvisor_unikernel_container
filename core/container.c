#include "mm.h"

struct containernet_data {
	void (*containernet_send) (void *handle, void *packet,
			  unsigned int packet_size);
	void *handle;
};

static struct containernet_data *containernet;


void
containernet_register (void (*containernet_send) (void *handle, void *packet,
				    unsigned int packet_size), void *handle)
{
	struct containernet_data *p;

	containernet = alloc (sizeof *p);
	containernet->containernet_send = containernet_send;
	containernet->handle = handle;
}

void containernet_write(char *buf, unsigned int size){
    // sizeを測ればよい？
    // 固定の配列と適切なサイズを送る
    containernet->containernet_send(containernet->handle, buf, size);
}
