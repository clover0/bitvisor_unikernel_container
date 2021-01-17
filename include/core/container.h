void containernet_register(
	void *handle,
	void (*containernet_send)(void *handle, void *packet, unsigned int packet_size),
	void (*containernet_recv)(void *handle, void *packet, unsigned int *packet_size));