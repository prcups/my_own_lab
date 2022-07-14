#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

static char buf[100005];

void handle_path(char * path, const char * prefix, const char * name) {
    int m = strlen(prefix);
    strcpy(path, prefix);
    if (path[m - 1] != '/') path[m++] = '/';
    strcpy(path + m, name);
}

void handle_error(const char * err_string) {
    printf(err_string);
    exit(127);
}

void handle_file(const char * source, const char * dest) {
    printf("%s -> %s\n", source, dest);
    int source_fd, dest_fd;
    if ((source_fd = open(source, O_RDONLY, 0644)) == -1) {
        handle_error("Source file error\n");
    }
    if (read(source_fd, buf, 100005) == -1) {
        handle_error("Read source file error\n");
    }
    if ((dest_fd = open(dest, O_WRONLY| O_CREAT | O_EXCL, 0644)) == -1) {
        handle_error("Dest file error\n");
    }
    if (write(dest_fd, buf, sizeof(buf)) == -1) {
        handle_error("Write dest file error\n");
    }
    close(source_fd);
    close(dest_fd);
}

void handle_dir(const char * source, const char * dest) {
    printf("%s -> %s\n", source, dest);
    DIR * dir;
    struct dirent * ptr;
    int i;
    dir = opendir(source);
    if (dir == NULL) {
        handle_error("Source dir error\n");
    }
    if (mkdir(dest, 0755) == -1) {
        handle_error("Destination dir error\n");
    }
    char source_path[512], dest_path[512];
    while((ptr = readdir(dir)) != NULL) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        struct stat *statbuf = malloc(sizeof(struct stat));
        handle_path(source_path, source, ptr->d_name);
        handle_path(dest_path, dest, ptr->d_name);
        if (lstat(source_path, statbuf) == -1) {
            handle_error("Get stat error\n");
        }
        if (S_ISLNK(statbuf->st_mode)) {
            printf("%s is a link\n", source_path);
        } else if (S_ISDIR(statbuf->st_mode)) {
            handle_dir(source_path, dest_path);
        } else if (S_ISREG(statbuf->st_mode)) {
            handle_file(source_path, dest_path);
        }
        free(statbuf);
    }
    closedir(dir);
}

int main(int argc, char ** argv) {
    if (argc != 3) {
        handle_error("Usage: cpdir sourcedir destinationdir\n");
    }
    handle_dir(argv[1], argv[2]);
    return 0;
}
