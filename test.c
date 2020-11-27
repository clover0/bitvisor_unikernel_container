#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

int test_net_io(void) {
	int sock;
	int itr_count = 10;
	struct sockaddr_in addr;
	struct timeval s_time, e_time;
	int sd;

	if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return -1;
	}

	// 送信先アドレスとポート番号を設定する
	// 受信プログラムと異なるあて先を設定しても UDP の場合はエラーにはならない
	addr.sin_family = AF_INET;
	addr.sin_port = htons(12049);
	// addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// addr.sin_addr.s_addr = inet_addr("192.168.255.255");
	// addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_addr.s_addr = inet_addr("192.168.0.181");

	gettimeofday(&s_time, NULL);
	for (int i = 0; i < itr_count; i++) {
		// sendto(sock, "HELLO", 5, 0, (struct sockaddr *)&addr, sizeof(addr));
		// パケットをUDPで送信
		if (sendto(sd, "HELLO", 5, 0,
				   (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("sendto");
			return -1;
		}
	}
	gettimeofday(&e_time, NULL);

	close(sd);
	printf("result %lf usec\n", (double)(e_time.tv_usec - s_time.tv_usec) / itr_count);

	return 0;
}

int main(void) {
	// unsigned long = 11241241433434;
	// unsigned long test = 83434;

	// printf("%f\n", (double)test / 1000000);
	test_net_io();
	return 0;
}