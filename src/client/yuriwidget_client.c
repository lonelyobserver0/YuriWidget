// === File: client/yuriwidget_client.c ===
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/yuriwidget.sock"

void send_command(const char *cmd) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
        write(sock, cmd, strlen(cmd));
    } else {
        perror("Connect failed");
    }
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s [show|hide|reload|js] <title> [js-code]\n", argv[0]);
        return 1;
    }

    char buffer[512];
    if (strcmp(argv[1], "js") == 0 && argc > 3) {
        snprintf(buffer, sizeof(buffer), "yuriwidget js %s %s", argv[2], argv[3]);
    } else {
        snprintf(buffer, sizeof(buffer), "yuriwidget %s %s", argv[1], argv[2]);
    }

    send_command(buffer);
    return 0;
}
