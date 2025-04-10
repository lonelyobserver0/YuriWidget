#include "socket_server.h"
#include "log.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static gboolean show_cb(gpointer data) {
    gtk_widget_show_all((GtkWidget *)data);
    return FALSE;
}

static gboolean hide_cb(gpointer data) {
    gtk_widget_hide((GtkWidget *)data);
    return FALSE;
}

void *socket_server_thread(void *arg) {
    AppContext *context = (AppContext *)arg;

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        log_message("Socket creation failed.");
        return NULL;
    }

    unlink(SOCKET_PATH);

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1 ||
        listen(sockfd, 5) == -1) {
        log_message("Socket bind/listen failed.");
        close(sockfd);
        return NULL;
    }

    log_message("Listening on socket...");

    while (1) {
        int client = accept(sockfd, NULL, NULL);
        if (client < 0) continue;

        char buf[256];
        ssize_t len = read(client, buf, sizeof(buf) - 1);
        if (len > 0) {
            buf[len] = '\0';

            if (strncmp(buf, "yuriwidget show ", 16) == 0) {
                if (strcmp(buf + 16, context->title) == 0)
                    g_idle_add(show_cb, context->window);
            } else if (strncmp(buf, "yuriwidget hide ", 16) == 0) {
                if (strcmp(buf + 16, context->title) == 0)
                    g_idle_add(hide_cb, context->window);
            }
        }

        close(client);
    }

    close(sockfd);
    return NULL;
}
