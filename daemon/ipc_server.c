#include "ipc_server.h"
#include <stdio.h>
#include <sys/time.h>
#include "setup.h"

#define CMD_START_SNIFFER 0
#define CMD_STOP_SNIFFER 1
#define CMD_CHANGE_INTERFACE 2
#define CMD_GET_SNIFFER_STATUS 3

static volatile sig_atomic_t ipc_running = 0;
static int listen_sock;

void* run_server(void* arg) {
    (void)arg;

    struct sockaddr_un addr;
    int client_sock;
    struct ipc_request req;
    ipc_response resp;
    fd_set readfds;
    struct timeval tv;

    unlink(SOCKET_NAME);

    listen_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_sock == -1) {
        perror("[ipc_server] socket error");
        return NULL;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    if (bind(listen_sock, (const struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("[ipc_server] bind error");
        close(listen_sock);
        return NULL;
    }

    if (listen(listen_sock, 10) == -1) {
        perror("[ipc_server] listen error");
        close(listen_sock);
        return NULL;
    }

    ipc_running = 1;
    while (ipc_running) {
        FD_ZERO(&readfds);
        FD_SET(listen_sock, &readfds);

        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int retval = select(listen_sock + 1, &readfds, NULL, NULL, &tv);

        if (retval == -1) {
            if (errno == EINTR) continue;
            perror("[ipc_server] select error");
            break;
        }

        if (retval == 0) {
            continue;
        }

        if (FD_ISSET(listen_sock, &readfds)) {
            client_sock = accept(listen_sock, NULL, NULL);
            if (client_sock < 0) {
                if (errno == EINTR) continue;
                if (ipc_running) perror("[ipc_server] accept error");
                continue;
            }

            if (read(client_sock, &req, sizeof(struct ipc_request)) > 0) {
                 resp = handle_command(req);
                 if (write(client_sock, &resp, sizeof(ipc_response)) < 0) {
                    perror("[ipc_server] write error");
                 }
            }
            close(client_sock);
        }
    }

    close(listen_sock);
    unlink(SOCKET_NAME);
    return NULL;
}

void stop_server(void) {
    ipc_running = 0;
    if (listen_sock > 0) {
        shutdown(listen_sock, SHUT_RDWR);
    }
}

ipc_response handle_command(struct ipc_request req) {
    switch (req.cmd) {
        case Start:
            if (is_sniffer_running()) {
                fprintf(stderr, "[ipc_server] Sniffer already running.\n");
                return ER_ALRYR;
            }
            if (setup_curr_if()) {
                fprintf(stderr, "[ipc_server] Failed to start sniffer on %s.\n", req.data);
                return ER_FSTAR;
            }
            fprintf(stderr, "[ipc_server] Sniffer started.");
            return 0;
        case Stop:;
            if (!is_sniffer_running()) {
                fprintf(stderr, "[ipc_server] Sniffer not running.\n");
                return ER_NOTRG;
            }
            if (stop_sniffer()) {
                fprintf(stderr, "[ipc_server] Failed to stop sniffer.\n");
                return ER_FSTOP;
            }
            fprintf(stderr, "[ipc_server] Sniffer stopped.\n");
            return 0;
        case SelectIf:
            if (is_sniffer_running()) {
                if (stop_sniffer()) {
                    fprintf(stderr, "[ipc_server] Failed to stop sniffer.\n");
                    return ER_FSTOP;
                }
            }

            if (setup_if(req.data)) {
                fprintf(stderr, "[ipc_server] Failed to start sniffer on new if.\n");
                return ER_FSTAR;
            }

            return 0;
        default:
            fprintf(stderr, "[ipc_server] Unknown command: %d\n", req.cmd);
            return ER_UNKNC;
    }
}