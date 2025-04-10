#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <glib.h>
#include <gio/gio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/un.h>
#include <sys/socket.h>

#define SOCKET_PATH "/tmp/yuriwidget.sock"
#define LOG_FILE "/tmp/yuriwidget.log"

static GtkWidget *main_window = NULL;
static WebKitWebView *webview = NULL;

static void log_message(const char *message) {
    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        fprintf(log_file, "%s\n", message);
        fclose(log_file);
    }
    fprintf(stderr, "%s\n", message);
}

static void apply_hyprland_window_settings(GtkWidget *window) {
    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_realize(window);

    GdkScreen *screen = gtk_widget_get_screen(window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(window, visual);
    }

    GdkWindow *gdk_window = gtk_widget_get_window(window);
    if (gdk_window) {
        GdkRGBA transparent = {0, 0, 0, 0};
        gdk_window_set_background_rgba(gdk_window, &transparent);
    }

    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
}

typedef struct {
    gboolean show;
} WindowCommand;

gboolean handle_window_command(gpointer data) {
    WindowCommand *cmd = (WindowCommand *)data;
    if (cmd->show) {
        gtk_widget_show(main_window);
    } else {
        gtk_widget_hide(main_window);
    }
    free(cmd);
    return FALSE;
}

static void *socket_server_thread(void *arg) {
    unlink(SOCKET_PATH);

    int sockfd;
    struct sockaddr_un server_addr;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        log_message("Error creating socket");
        return NULL;
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1) {
        log_message("Error binding socket");
        close(sockfd);
        return NULL;
    }

    if (listen(sockfd, 5) == -1) {
        log_message("Error listening on socket");
        close(sockfd);
        return NULL;
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
            log_message(buffer);

            if (strncmp(buffer, "show", 4) == 0) {
                WindowCommand *cmd = malloc(sizeof(WindowCommand));
                cmd->show = TRUE;
                g_idle_add(handle_window_command, cmd);
            } else if (strncmp(buffer, "hide", 4) == 0) {
                WindowCommand *cmd = malloc(sizeof(WindowCommand));
                cmd->show = FALSE;
                g_idle_add(handle_window_command, cmd);
            }
        }

        close(client_fd);
    }

    close(sockfd);
    return NULL;
}

static GtkWidget *create_yuriwidget_window(const char *title, const char *url) {
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), title);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);

    webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_uri(webview, url);
    gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(webview));

    gtk_widget_show_all(main_window);

    return main_window;
}

static void destroy(GtkWidget *window, gpointer data) {
    unlink(SOCKET_PATH);
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    const char *url = "https://example.com";
    const char *title = "yuriwidget";

    static struct option long_options[] = {
        {"config-file", required_argument, 0, 'c'},
        {"title", required_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "c:t:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c':
                url = optarg;
                break;
            case 't':
                title = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-c|--config-file <url>] [-t|--title <window title>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    pthread_t socket_thread;
    pthread_create(&socket_thread, NULL, socket_server_thread, NULL);

    GtkWidget *window = create_yuriwidget_window(title, url);
    apply_hyprland_window_settings(window);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy), NULL);

    gtk_main();
    pthread_join(socket_thread, NULL);

    return 0;
}
