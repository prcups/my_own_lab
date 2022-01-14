#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/ether.h>

int main() {
	unsigned char buf[1024] = {0};
	int sock_raw_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));
	if (sock_raw_fd < 0) {
		perror("socket");
		return -1;
	}
	while (1) {
		int len = recvfrom(sock_raw_fd, buf, sizeof(buf), 0, NULL, NULL);
		for (int i = 0; i < 22; ++i) {
			printf("%02hhx ", buf[i]);
		}
		putchar('\n');
		for (int i = 22; i < 42; ++i) {
			printf("%02hhx ", buf[i]);
		}
		putchar('\n');
		for (int i = 42; i < len; ++i) {
			if (buf[i] >= 32 && buf[i] <= 126) {
				putchar(buf[i]);
			} else {
				printf(" |%d| ", buf[i]);
			}
		}
		putchar('\n');
		putchar('\n');
	}
}
