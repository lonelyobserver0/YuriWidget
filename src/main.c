#include <gtk/gtk.h>
#include "args.h"
#include "window.h"
#include "socket_server.h"
#include "log.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    parse_args(argc, argv);

    GtkWidget *main_window = create_yuriwidget_window();
    AppContext *context = init_app_context(main_window, window_title);

    pthread_t socket_thread;
    pthread_create(&socket_thread, NULL, socket_server_thread, context);

    apply_hyprland_window_settings(main_window);

    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_main();

    unlink(SOCKET_PATH);
    pthread_join(socket_thread, NULL);

    return 0;
}
