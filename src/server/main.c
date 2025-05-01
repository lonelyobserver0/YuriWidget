#include <gtk/gtk.h>
#include <pthread.h>
#include "server/window.h"
#include "server/config.h"
#include "server/socket_server.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        g_printerr("Usage: %s <config.json>\n", argv[0]);
        return 1;
    }

    gtk_init(&argc, &argv);

    Config *cfg = config_load_from_file(argv[1]);
    if (!cfg) return 1;

    AppContext *ctx = create_app_context(cfg);
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, start_socket_server, ctx);

    gtk_main();

    destroy_app_context(ctx);
    return 0;
}
