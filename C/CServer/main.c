#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096
const char * response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";

void err(const char * str) {
	printf("%s failed\n", str);
	exit(-1);
}

void http_server() {
	int listen_fd, conn_fd;
	int opt = 1;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	char buf[BUFFER_SIZE];

	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		err("socket");
	}
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
		err("setsockopt");
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(80);
	if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		err("bind");
	}

	if (listen(listen_fd, 3) < 0) {
		err("listen");
	}
	while (1) {
		if ((conn_fd =
			accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len))
			< 0) {
				err("accept");
			}

		read(conn_fd, buf, BUFFER_SIZE);
		printf("%s\n", buf);
		write(conn_fd, response, strlen(response));
		close(conn_fd);
	}
}

void https_server() {

}

int main() {
	// pid_t http_pid, https_pid;
	// http_pid = fork();
	// if (http_pid < 0) {
	// 	printf("failed to start http handler\n");
	// } else if (http_pid == 0) {
	// 	http_server();
	// } else {
	// 	https_pid = fork();
	// 	if (https_pid < 0) {
	// 		printf("failed to start https handler\n");
	// 	} else if (https_pid == 0) {
	// 		https_server();
	// 	}
	// }
	// int http_st, https_st;
	// waitpid(http_pid, http_st, 0);
	// waitpid(https_pid, https_st, 0);
	http_server();
	return 0;
}
