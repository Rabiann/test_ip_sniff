#include "commands.h"

int main(int argc, char* argv[]) {
    struct ipc_request req;
fprintf(stderr, "=== MAIN STARTED ===\n");
    if (parse_command(&req, argc, argv)) {
        if (run_client(req)) {
            fprintf(stderr, "IPC CLIENT ERROR");
            return 1;
        }
    }
        
    return 0;
}