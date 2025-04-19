// === File: src/server.c ===
#include "server.h"
#include <gio/gio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_PATH "/tmp/yuriwidget.sock"

static gboolean show_cb(gpointer data) {
    gtk_widget_show_all(((AppContext *)data)->window);
    return FALSE;
}
static gboolean hide_cb(gpointer data) {
    gtk_widget_hide(((AppContext *)data)->window);
    return FALSE;
}

void *start_socket_server(void *arg) {
    AppContext *ctx = (AppContext *)arg;

    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);
    bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    listen(sockfd, 5);

    while (1) {
        int client = accept(sockfd, NULL, NULL);
        if (client < 0) continue;

        char buf[512] = {0};
        read(client, buf, sizeof(buf) - 1);

        if (g_str_has_prefix(buf, "yuriwidget show ")) {
            if (g_strcmp0(buf + 16, ctx->config->title) == 0)
                g_idle_add(show_cb, ctx);
        } else if (g_str_has_prefix(buf, "yuriwidget hide ")) {
            if (g_strcmp0(buf + 16, ctx->config->title) == 0)
                g_idle_add(hide_cb, ctx);
        } else if (g_str_has_prefix(buf, "yuriwidget reload ")) {
            if (g_strcmp0(buf + 18, ctx->config->title) == 0)
                g_idle_add((GSourceFunc)reload_webview, ctx);
        } else if (g_str_has_prefix(buf, "yuriwidget js ")) {
            char *rest = buf + 14;
            char *title = strtok(rest, " ");
            char *js = strtok(NULL, "");
            if (g_strcmp0(title, ctx->config->title) == 0 && js)
                g_idle_add_full(G_PRIORITY_DEFAULT, (GSourceFunc)(void *)execute_js, g_strdup(js), g_free);
        }

        close(client);
    }
    return NULL;
}