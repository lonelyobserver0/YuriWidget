#include "config.h"
#include "window.h"
#include "server.h"
#include <pthread.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        g_printerr("Usage: %s config.json\n", argv[0]);
        return 1;
    }

    Config *cfg = config_load_from_file(argv[1]);
    if (!cfg) return 1;

    AppContext *ctx = create_app_context(cfg);

    pthread_t thread;
    pthread_create(&thread, NULL, start_socket_server, ctx);

    gtk_main();
    destroy_app(ctx);
    return 0;
}
