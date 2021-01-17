#include "mm.h"

struct containernet_data {
	void (*containernet_send)(void *handle, void *packet,
							  unsigned int packet_size);
	void (*containernet_recv)(void *handle, void *packet,
							  unsigned int packet_size, unsigned int *r_packet_size);
	void *handle;
};

static struct containernet_data *containernet;

void containernet_register(
	void *handle,
	void (*containernet_send)(void *handle, void *packet, unsigned int packet_size),
	void (*containernet_recv)(void *handle, void *packet, unsigned int packet_size, unsigned int *r_packet_size)) {
	struct containernet_data *p;

	containernet = alloc(sizeof *p);
	containernet->containernet_send = containernet_send;
	containernet->containernet_recv = containernet_recv;
	containernet->handle = handle;
}

void containernet_write(char *buf, unsigned int size) {
	containernet->containernet_send(containernet->handle, buf, size);
}

void containernet_read(char *buf, unsigned int size, unsigned int *r_size) {
	containernet->containernet_recv(containernet->handle, buf, size, r_size);
}
