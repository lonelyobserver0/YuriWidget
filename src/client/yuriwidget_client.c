#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/yuriwidget.sock"

void send_command(const char *cmd, const char *title) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return;
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(sock);
        return;
    }

    char message[512];
    snprintf(message, sizeof(message), "%s %s", cmd, title);
    write(sock, message, strlen(message));
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [show|hide] <title>\n", argv[0]);
        return 1;
    }

    send_command(argv[1], argv[2]);
    return 0;
}
