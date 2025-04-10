#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <glib.h>
#include <gio/gio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/yuriwidget.sock"
#define LOG_FILE "/tmp/yuriwidget.log"

static void log_message(const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        fprintf(log_file, "%s\n", message);
        fclose(log_file);
    }
    fprintf(stderr, "%s\n", message);
}

static void apply_hyprland_window_settings(GtkWidget *window) {
    GdkWindow *gdk_window = gtk_widget_get_window(window);
    if (gdk_window) {
        // Imposta la finestra come trasparente
        GdkRGBA transparent = {0, 0, 0, 0};
        gdk_window_set_background_rgba(gdk_window, &transparent);

        // Altri settaggi specifici per Hyprland possono essere aggiunti qui
    }
}

static void socket_server_thread(void *arg) {
    int sockfd;
    struct sockaddr_un server_addr;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        log_message("Error creating socket");
        return;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        log_message("Error binding socket");
        close(sockfd);
        return;
    }

    if (listen(sockfd, 5) == -1) {
        log_message("Error listening on socket");
        close(sockfd);
        return;
    }

    log_message("Server listening on socket...");

    while (1) {
        int client_fd = accept(sockfd, NULL, NULL);
        if (client_fd == -1) {
            log_message("Error accepting connection");
            continue;
        }

        char buffer[256];
        ssize_t len = read(client_fd, buffer, sizeof(buffer) - 1);
        if (len > 0) {
            buffer[len] = '\0';
            log_message(buffer); // Log command received

            // Gestione dei comandi IPC
            if (strncmp(buffer, "show ", 5) == 0) {
                const char *title = buffer + 5;
                // Gestisci il comando per mostrare la finestra con il titolo specificato
                log_message("Show window with title: ");
                log_message(title);
            } else if (strncmp(buffer, "hide ", 5) == 0) {
                const char *title = buffer + 5;
                // Gestisci il comando per nascondere la finestra con il titolo specificato
                log_message("Hide window with title: ");
                log_message(title);
            }
        }

        close(client_fd);
    }

    close(sockfd);
}

static GtkWidget *create_yuriwidget_window() {
    GtkWidget *main_window;
    WebKitWebView *webview;

    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "yuriwidget");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);

    webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(webview));

    gtk_widget_show_all(main_window);

    return main_window;
}

static void destroy(GtkWidget *window, gpointer data) {
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Thread per il server socket IPC
    pthread_t socket_thread;
    pthread_create(&socket_thread, NULL, (void *(*)(void *))socket_server_thread, NULL);

    GtkWidget *main_window = create_yuriwidget_window();

    // Impostazioni specifiche per Hyprland
    apply_hyprland_window_settings(main_window);

    g_signal_connect(main_window, "destroy", G_CALLBACK(destroy), NULL);

    gtk_main();

    pthread_join(socket_thread, NULL);

    return 0;
}
