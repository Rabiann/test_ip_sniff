#include "ipc_client.h"

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
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(SOCKET_NAME) - 1);

    if (connect(client_sock, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un))) {
        if (errno == ERROR_D_NOT_STARTED) {
            system("./snifferd");
            return 0;
        }

        perror("[ipc_client] connect error");
        return -2;
    }

    if (write(client_sock, &req, sizeof(struct ipc_request))) {
        perror("[ipc_client] write error");
        return -2;
    }

    if (read(client_sock, &resp, sizeof(ipc_response))) {
        perror("[ipc_client] read error");
        return -2;
    }

    close(client_sock);
    return resp;
}

int start_d() {
    if (system("./snifferd")) {
        perror("[ipc_client] start_d system error");
        return -1;
    }

    printf("Daemon started.");
    return 0;
}