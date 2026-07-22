CC = gcc
CFLAGS  	= -Wall -Wextra -O2 -g -Idaemon
LDFLAGS 	= -lpcap

DAEMON_BIN 	= snifferd
DAEMON_CLI 	= snifferctl

DAEMON_SRC 	= daemon/main.c daemon/sniffer.c daemon/ipc_server.c
CLI_SRC 	= cli/main.c cli/ipc_client.c cli/commands.c

DAEMON_OBJ 	= $(DAEMON_SRC:.c=.o)
CLI_OBJ 	= $(CLI_SRC:.c=.o)

all: $(DAEMON_BIN) $(DAEMON_CLI)

$(DAEMON_BIN): $(DAEMON_OBJ)
	$(CC) $(DAEMON_OBJ) -o $@ $(LDFLAGS)

$(DAEMON_CLI): $(CLI_OBJ)
	$(CC) $(CLI_OBJ) -o $@

clean:
	rm -f daemon/*.o snifferd
	rm -f cli/*.o snifferctl


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean