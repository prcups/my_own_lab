#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	int pid, c = 0, status;
	pid = fork();
	if (pid == 0) {
		char a[50][50];
		for (int i = 0; i < 50; ++i) {
			for (int j = 0; j < 50; ++j) {
				a[i][j] = 'a';
				putchar(a[i][j]);
			}
			putchar('\n');
		}
		printf("%d %d %d %d %d %d\n", getpid(), getppid(), getuid(), geteuid(), getgid(), getegid());
	} else {
		wait(&status);
		printf("%d %d %d %d %d %d\n", getpid(), getppid(), getuid(), geteuid(), getgid(), getegid());
	}
}
