#include "commands.h"

static char* Start_cmd  = "start";
static char* Stop_cmd   = "stop";
static char* Show_cmd1  = "show";
static char* Show_cmd2  = "count";
static char* Select_cmd1 = "select";
static char* Select_cmd2 = "select";
static char* Show_stat  = "stat";
static char* Show_help  = "--help";

int parse_command(struct ipc_request* req, int argc, char* argv[]) {
    if (argc < 2)
        show_help();

    if (argc == 2 && strcmp(argv[1], Start_cmd) == 0) { 
        req->cmd = Start;
        return 1;
    }

    if (argc == 2 && strcmp(argv[1], Stop_cmd) == 0) {
        req->cmd = Stop;
        return 1;
    }

    if (argc == 4 && strcmp(argv[1], Show_cmd1)  == 0 &&
        strcmp(argv[3], Show_cmd2) == 0) {
        // TODO: gather stats for ip addr
        return 1;
    }

    if (argc == 4 && strcmp(argv[1], Select_cmd1)  == 0 &&
        strcmp(argv[2], Select_cmd2) == 0) {
        
        req->cmd = SelectIf;
        strncpy(req->data, argv[2], sizeof(req->data) - 1);
        req->data[14] = 0;
        return 1;
    }

    if (argc < 4 && strcmp(argv[1], Show_stat) == 0) {
        if (argc == 3) {
            show_address_stats(argv[2]);   
        } else {
            show_all_stats();
        }

        return 1;
    } 

    if (argc == 2 && strcmp(argv[1], Show_help) == 0) {
        show_help();
    } 

    show_help();
    return 0;
}

void show_all_stats() {}

void show_address_stats(char* iface) {}

void show_help() {}