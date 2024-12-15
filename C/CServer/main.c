#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE 1024

static const char *res_not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found";
static const char *default_file_name = "index.html";

void err(const char * str) {
	printf("%s failed\n", str);
	exit(-1);
}

void http_write_str(int fd, const char*str) {
	write(fd, str, strlen(str));
}

void https_write_str(SSL* ssl, const char*str) {
	SSL_write(ssl, str, strlen(str));
}

void handle_http(int conn_fd) {
	char buf[BUFFER_SIZE], host[100], method[10], url[100], ver[10];
	read(conn_fd, buf, BUFFER_SIZE);
	sscanf(buf, "%s %s %s", method, url, ver);
	const char *host_pos = strstr(buf, "Host: ");
	if (host_pos != NULL) {
		sscanf(host_pos, "Host: %s", host);
	}

	http_write_str(conn_fd, "HTTP/1.0 301 Moved Permanently\r\nLocation: https://");
	http_write_str(conn_fd, host);
	http_write_str(conn_fd, url);
	http_write_str(conn_fd, "\r\nContent-Type: text/html\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");
	close(conn_fd);
}

SSL* load_ssl(int conn_fd) {
	SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    if(SSL_CTX_use_certificate_file(ctx, "ca.crt", SSL_FILETYPE_PEM) <= 0) {
		close(conn_fd);
        err("public key");
    }
    if(SSL_CTX_use_PrivateKey_file(ctx, "ca.key", SSL_FILETYPE_PEM) <= 0) {
        close(conn_fd);
		err("private key");
    }
    if(SSL_CTX_check_private_key(ctx) <= 0) {
        close(conn_fd);
		err("private key check");
    }

    SSL *ssl = SSL_new(ctx);

    if(ssl == NULL) {
		close(conn_fd);
		SSL_free(ssl);
        err("ssl create");
    }

    if(SSL_set_fd(ssl, conn_fd) == 0) {
		close(conn_fd);
		SSL_free(ssl);
        err("ssl set fd");
    }
    return ssl;
}

void handle_https(int conn_fd) {
	char buf[BUFFER_SIZE], method[10], url[100], ver[10];
	int has_range = 0, range_start, range_end;

	SSL *ssl = load_ssl(conn_fd);
	if (SSL_accept(ssl) == -1) {
		close(conn_fd);
		err("ssl accept");
	}

	SSL_read(ssl, buf, BUFFER_SIZE);
	sscanf(buf, "%s %s %s", method, url, ver);
	printf("%s %s %s\n", method, url, ver);
	if (strcmp(method, "GET") == 0 && strstr(ver, "HTTP")) {
		const char* file_name;
		if (strlen(url) == 1) file_name = default_file_name;
		else file_name = url + 1;

		struct stat sbuf;
		if (stat(file_name, &sbuf) < 0) {
			https_write_str(ssl, res_not_found);
			close(conn_fd);
			SSL_free(ssl);
			return;
		}

		const char *range_pos = strstr(buf, "Range: ");
		if (range_pos != NULL) {
			has_range = 1;
			int n = sscanf(range_pos, "Range: bytes=%d-%d", range_start, range_end);
			if (n == 1) range_end = sbuf.st_size;
		}

		https_write_str(ssl, "HTTP/1.0 200 OK\r\n");
		sprintf(buf, "Content-length: %ld\r\n", sbuf.st_size);
		https_write_str(ssl, buf);
		if (strstr(file_name, ".html"))
			https_write_str(ssl, "Content-type: text/html\r\n\r\n");
		else https_write_str(ssl, "Content-type: text/plain\r\n\r\n");

		int file_fd = open(file_name, O_RDONLY);
		char *content = mmap(0, sbuf.st_size, PROT_READ,
								MAP_PRIVATE, file_fd, 0);
		if (has_range) {
			SSL_write(ssl, content + range_start, range_end - range_start + 1);
		} else SSL_write(ssl, content, sbuf.st_size);
		munmap(content, sbuf.st_size);
		close(file_fd);
	}
	close(conn_fd);
	SSL_free(ssl);
}

void server(int ssl) {
	int listen_fd;
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		err("socket");
	}
	int opt = 1;
	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0) {
		err("setsockopt");
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(ssl ? 443 : 80);
	if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		err("bind");
	}

	if (listen(listen_fd, 3) < 0) {
		err("listen");
	}

	while (1) {
		int conn_fd;
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		if ((conn_fd =
			accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len))
			< 0) {
				err("accept");
			}
		int pid = fork();
		if (pid == 0) {
			if (ssl) handle_https(conn_fd);
			else handle_http(conn_fd);
			return;
		}
	}
}

int main() {
	int http_pid, https_pid;
	http_pid = fork();
	if (http_pid == 0) {
		server(0);
	} else {
		https_pid = fork();
		if (https_pid == 0) {
			server(1);
		} else {
			int http_st, https_st;
			waitpid(http_pid, &http_st, 0);
			waitpid(https_pid, &https_st, 0);
		}
	}
	return 0;
}
