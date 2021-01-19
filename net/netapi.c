/*
 * Copyright (c) 2007, 2008 University of Tsukuba
 * Copyright (c) 2014 Igel Co., Ltd
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the University of Tsukuba nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <core/config.h>
#include <core/initfunc.h>
#include <core/list.h>
#include <core/mm.h>
#include <core/panic.h>
#include <core/spinlock.h>
#include <core/string.h>
#include <core/thread.h>
#include <core/tty.h>
#include <core/container.h>
#include <net/netapi.h>

struct netdata {
	struct netfunc *func;
	void *handle;
	bool tty;
	void *tty_phys_handle;
	struct nicfunc *tty_phys_func;
	void *container_phys_handle;
	struct nicfunc *container_phys_func;
	unsigned char mac_address[6];
};

struct netlist {
	struct netlist *next;
	char *name;
	struct netfunc *func;
	void *param;
};

struct net_null_data {
	void *phys_handle;
	struct nicfunc *phys_func;
	void *virt_handle;
	struct nicfunc *virt_func;
};

struct net_pass_data2 {
	void *handle;
	struct nicfunc *func;
};

struct net_pass_data {
	struct net_pass_data2 phys, virt;
};

struct net_container_data {
	void *phys_handle, *virt_handle;
	struct nicfunc *phys_func, *virt_func;
	void *input_arg;
};

struct container_packet {
	LIST1_DEFINE (struct container_packet);
	void *packet;
	unsigned int size;
};

static struct netlist *netlist_head = NULL;

static LIST1_DEFINE_HEAD (struct container_packet, container_packet_list);
static spinlock_t container_packet_lock;

static void
netapi_net_null_recv_callback (void *handle, unsigned int num_packets,
			       void **packets, unsigned int *packet_sizes,
			       void *param, long *premap)
{
	/* Do nothing. */
}

static void *
netapi_net_null_new_nic (char *arg, void *param)
{
	struct net_null_data *p;

	p = alloc (sizeof *p);
	return p;
}

static bool
netapi_net_null_init (void *handle, void *phys_handle,
		      struct nicfunc *phys_func, void *virt_handle,
		      struct nicfunc *virt_func)
{
	struct net_null_data *p = handle;

	p->phys_handle = phys_handle;
	p->phys_func = phys_func;
	p->virt_handle = virt_handle;
	p->virt_func = virt_func;
	return true;
}

static void
netapi_net_null_start (void *handle)
{
	struct net_null_data *p = handle;

	p->phys_func->set_recv_callback (p->phys_handle,
					 netapi_net_null_recv_callback, NULL);
	if (p->virt_func)
		p->virt_func->set_recv_callback (p->virt_handle,
						 netapi_net_null_recv_callback,
						 NULL);
}

static void
netapi_net_pass_recv_callback (void *handle, unsigned int num_packets,
			       void **packets, unsigned int *packet_sizes,
			       void *param, long *premap)
{
	struct net_pass_data2 *p = param;

	p->func->send (p->handle, num_packets, packets, packet_sizes, true);
}

static void *
netapi_net_pass_new_nic (char *arg, void *param)
{
	struct net_pass_data *p;

	p = alloc (sizeof *p);
	return p;
}

static bool
netapi_net_pass_init (void *handle, void *phys_handle,
		      struct nicfunc *phys_func, void *virt_handle,
		      struct nicfunc *virt_func)
{
	struct net_pass_data *p = handle;

	if (!virt_func)
		return false;
	p->phys.handle = phys_handle;
	p->phys.func = phys_func;
	p->virt.handle = virt_handle;
	p->virt.func = virt_func;
	return true;
}

static void
netapi_net_pass_start (void *handle)
{
	struct net_pass_data *p = handle;

	p->phys.func->set_recv_callback (p->phys.handle,
					 netapi_net_pass_recv_callback,
					 &p->virt);
	p->virt.func->set_recv_callback (p->virt.handle,
					 netapi_net_pass_recv_callback,
					 &p->phys);
}

void
net_register (char *netname, struct netfunc *func, void *param)
{
	struct netlist *p;

	p = alloc (sizeof *p);
	p->name = netname;
	p->func = func;
	p->param = param;
	p->next = netlist_head;
	netlist_head = p;
}

struct netdata *
net_new_nic (char *arg_net, bool tty)
{
	int i;
	char *arg = NULL;
	void *param;
	struct netlist *p;
	struct netfunc *func;
	struct netdata *handle;

	if (!arg_net)
		arg_net = "";
	for (p = netlist_head; p; p = p->next) {
		for (i = 0;; i++) {
			if (p->name[i] == '\0') {
				if (arg_net[i] == ':') {
					arg = &arg_net[i + 1];
					goto matched;
				}
				if (arg_net[i] == '\0')
					goto matched;
				break;
			}
			if (arg_net[i] != p->name[i])
				break;
		}
	}
	panic ("net_new_nic: invalid name net=%s", arg_net);
matched:
	func = p->func;
	param = p->param;
	handle = alloc (sizeof *handle);
	handle->func = func;
	handle->tty = tty;
	handle->tty_phys_func = NULL;
	handle->handle = handle->func->new_nic (arg, param);
	return handle;
}

static void
net_tty_send (void *tty_handle, void *packet, unsigned int packet_size)
{
	struct netdata *handle = tty_handle;
	char *pkt;

	pkt = packet;
	memcpy (pkt + 0, config.vmm.tty_mac_address, 6);
	memcpy (pkt + 6, handle->mac_address, 6);
	handle->tty_phys_func->send (handle->tty_phys_handle, 1, &packet,
				     &packet_size, false);
}

static void
net_container_send(void *containernet_handle, void *packet, unsigned int packet_size) {
	struct netdata *handle = containernet_handle;

	handle->container_phys_func->send(handle->container_phys_handle, 1, &packet,
									  &packet_size, false);
}

static void
net_container_recv(void *containernet_handle, void *packet, unsigned int size, unsigned int *r_size) {
	struct container_packet *p;

	*r_size = 0;

	spinlock_lock (&container_packet_lock);
	if ((p = LIST1_POP(container_packet_list))) {
		if (p->size > 0) {
			// printf("pop queue: size=%d, actual_size=%d\n", size, p->size);
			memcpy(packet, p->packet, size); // TODO: sizeのほう
			*r_size = p->size;
		}
	}
	spinlock_unlock (&container_packet_lock);
	// free (p);
}

void
net_container_packet_add (struct container_packet *p)
{
	spinlock_lock (&container_packet_lock);
	LIST1_ADD (container_packet_list, p);
	spinlock_unlock (&container_packet_lock);
}

void
container_net_main_poll (void *handle)
{
	struct net_container_data *p = handle;

	if (p->phys_func->poll)
		p->phys_func->poll (p->phys_handle);
}

static void containernet_main_task (void *handle)
{
	container_net_main_poll(handle);
}

static void
container_net_thread (void *arg)
{
	struct net_container_data *p = arg;

	for (;;) {
		containernet_main_task (p);
		schedule ();
	}
}

static void *
net_container_new_nic (char *arg, void *param)
{
	struct net_container_data *p;
	static int flag;

	if (flag)
		panic ("net=container does not work with multiple network"
		       " interfaces");
	flag = 1;
	p = alloc (sizeof *p);
	return p;
}

static bool
net_container_init (void *handle, void *phys_handle, struct nicfunc *phys_func,
	     void *virt_handle, struct nicfunc *virt_func)
{
	struct net_container_data *p = handle;

	p->phys_handle = phys_handle;
	p->phys_func = phys_func;
	p->virt_handle = virt_handle;
	p->virt_func = virt_func;
	return true;
}

static void
net_container_main_input_queue (struct net_container_data *p, void **packets,
		      unsigned int *packet_sizes, unsigned int num_packets)
{
	struct container_packet *data;
	unsigned int i;

	for (i = 0; i < num_packets; i++) {
		data = alloc (sizeof *data);
		data->packet = alloc (packet_sizes[i]);
		data->size = packet_sizes[i];
		memcpy (data->packet, packets[i], packet_sizes[i]);
		net_container_packet_add (data);
	}
}

// 物理NICがパケットを受信したときのcontainerネットワーク用コールバック関数
static void
net_container_phys_recv (void *handle, unsigned int num_packets, void **packets,
		  unsigned int *packet_sizes, void *param, long *premap)
{
	struct net_container_data *p = param;

	net_container_main_input_queue (p, packets, packet_sizes, num_packets);
}

static void
net_container_start (void *handle)
{
	struct net_container_data *p = handle;

	p->phys_func->set_recv_callback (p->phys_handle, net_container_phys_recv, p);
	
	thread_new (container_net_thread, p, VMM_STACKSIZE);
	
}

bool
net_init (struct netdata *handle, void *phys_handle, struct nicfunc *phys_func,
	  void *virt_handle, struct nicfunc *virt_func)
{
	if (!handle->func->init (handle->handle, phys_handle, phys_func,
				 virt_handle, virt_func))
		return false;
	if (handle->tty) {
		handle->tty_phys_handle = phys_handle;
		handle->tty_phys_func = phys_func;
	}
	// for container
	handle->container_phys_handle = phys_handle;
	handle->container_phys_func = phys_func;
	return true;
}

void
net_start (struct netdata *handle)
{
	struct nicinfo info;

	if (handle->tty) {
		handle->tty_phys_func->get_nic_info (handle->tty_phys_handle,
						     &info);
		memcpy (handle->mac_address, info.mac_address,
			sizeof handle->mac_address);
		tty_udp_register (net_tty_send, handle);
	}

	// for container
	containernet_register(handle, net_container_send, net_container_recv);

	handle->func->start (handle->handle);
}

long
net_premap_recvbuf (struct netdata *handle, void *buf, unsigned int len)
{
	if (!handle->func->premap_recvbuf)
		return 0;
	return handle->func->premap_recvbuf (handle->handle, buf, len);
}

static void
netapi_init (void)
{
	static struct netfunc net_null = {
		.new_nic = netapi_net_null_new_nic,
		.init = netapi_net_null_init,
		.start = netapi_net_null_start,
	};
	static struct netfunc net_pass = {
		.new_nic = netapi_net_pass_new_nic,
		.init = netapi_net_pass_init,
		.start = netapi_net_pass_start,
	};
	static struct netfunc net_container = {
		.new_nic = net_container_new_nic,
		.init = net_container_init,
		.start = net_container_start,
	};

	LIST1_HEAD_INIT (container_packet_list);

	net_register ("", &net_null, NULL);
	net_register ("pass", &net_pass, NULL);
	net_register ("container", &net_container, NULL);
}

INITFUNC ("driver0", netapi_init);
