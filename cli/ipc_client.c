#include "ipc_client.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

ipc_response run_client(struct ipc_request req) {
    struct sockaddr_un addr;
    ipc_response resp;

    int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("[ipc_client] socket error");
        return 1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    if (connect(client_sock, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un)) != 0) {
        if (errno == ECONNREFUSED || errno == ENOENT) {
            printf("Daemon not running. Starting it now...\n");
            if (start_d() != 0) {
                fprintf(stderr, "Failed to start daemon.\n");
                return -1;
            }

            return 0;
        } else {
            perror("[ipc_client] connect error");
            return -2;
        }
    }

    if (write(client_sock, &req, sizeof(struct ipc_request)) == -1) {
        perror("[ipc_client] write error");
        close(client_sock);
        return -2;
    }

    if (read(client_sock, &resp, sizeof(ipc_response)) == -1) {
        perror("[ipc_client] read error");
        close(client_sock);
        return -2;
    }

    close(client_sock);
    return resp;
}

int start_d() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("[ipc_client] fork error");
        return -1;
    }

    if (pid > 0) {
        sleep(1);
        return 0;
    }

    if (setsid() < 0) {
        perror("[ipc_client] setsid error");
        exit(EXIT_FAILURE);
    }

    int fd = open("/dev/null", O_RDWR);
    if(fd != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if(fd > 2) {
            close(fd);
        }
    }

    execlp("./snifferd", "snifferd", (char *)NULL);

    perror("[ipc_client] execlp error");
    exit(EXIT_FAILURE);
}