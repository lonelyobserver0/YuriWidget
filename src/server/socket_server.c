#include "src/server/include/socket_server.h"
#include "src/server/include/window.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_PATH "/tmp/yuriwidget.sock"

void *start_socket_server(void *data) {
    AppContext *ctx = (AppContext *)data;
    int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;

    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    bind(server_sock, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_sock, 5);

    char buf[256];
    while (1) {
        int client = accept(server_sock, NULL, NULL);
        int len = read(client, buf, sizeof(buf) - 1);
        buf[len] = '\0';

        if (strstr(buf, "show") == buf) {
            show_window(ctx);
        } else if (strstr(buf, "hide") == buf) {
            hide_window(ctx);
        }

        close(client);
    }

    close(server_sock);
    return NULL;
}
