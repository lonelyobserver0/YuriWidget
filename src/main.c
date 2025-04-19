// === File: src/main.c ===
#include <gtk/gtk.h>
#include <pthread.h>
#include "config.h"
#include "window.h"
#include "server.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    if (argc < 2) {
        g_printerr("Usage: %s config.json\n", argv[0]);
        return 1;
    }

    YuriConfig *cfg = load_config_from_file(argv[1]);
    if (!cfg) {
        g_printerr("Failed to load config.\n");
        return 1;
    }

    AppContext *ctx = create_app_context(cfg);
    g_signal_connect(ctx->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, start_socket_server, ctx);

    gtk_main();

    pthread_join(thread, NULL);
    destroy_app(ctx);
    return 0;
}