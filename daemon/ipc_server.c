#include "ipc_server.h"

int run_server() {
    struct sockaddr_un addr;
    int client_sock;
    struct ipc_request req;
    ipc_response resp;

    unlink(SOCKET_NAME);

    int listen_sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (listen_sock == -1) {
        perror("[ipc_server] socker error: ");
        return 1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    if (bind(listen_sock, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un))) {
        perror("[ipc_server] bind error: ");
        return 1;
    }

    if (listen(listen_sock, 10)) {
        perror("[ipc_server] listen error: ");
        return 1;
    }

    while (1) {
        client_sock = accept(listen_sock, NULL, NULL);
        if (client_sock) {
            perror("[ipc_server] accepr error: ");
            return 1;
        }

        if (read(client_sock, &req, sizeof(struct ipc_request))) {
            perror("[ipc_server] read error: ");
            return 1;
        }

        resp = handle_command(req);

        if (write(client_sock, &resp, sizeof(ipc_response))) {
            perror("[ipc_server] write error: ");
            return 1;
        }

        close(client_sock);
    }

    close(listen_sock);
    return 0;
}   

// TODO: implement commands
// - start
// - stop
// - change interface
// - show ip count from address
ipc_response handle_command(struct ipc_request req) {
    return 0;
}