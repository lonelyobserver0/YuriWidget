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
#include <sys/socket.h>
#include <sys/un.h>


#define SOCKET_PATH "/tmp/yuriwidget.sock"
#define LOG_FILE "/tmp/yuriwidget.log"

static char *config_url = NULL;
static char *window_title = "yuriwidget";
static int win_width = 800;
static int win_height = 600;
static int win_x = -1;
static int win_y = -1;

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
        GdkRGBA transparent = {0, 0, 0, 0};
        gdk_window_set_background_rgba(gdk_window, &transparent);
    }
}

static void *socket_server_thread(void *arg) {
    AppContext *context = (AppContext *)arg;
    int sockfd;
    struct sockaddr_un server_addr;

    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd == -1) {
        log_message("Error creating socket");
        return;
    }

    unlink(SOCKET_PATH);

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
            log_message(buffer);

            if (strncmp(buffer, "show ", 5) == 0) {
                const char *title = buffer + 5;
                log_message("Show window with title: ");
                log_message(title);

                if (strcmp(title, context->title) == 0) {
                    g_idle_add(show_window, context->window);
                }

            } else if (strncmp(buffer, "hide ", 5) == 0) {
                const char *title = buffer + 5;
                log_message("Hide window with title: ");
                log_message(title);

                if (strcmp(title, context->title) == 0) {
                    g_idle_add(hide_window, context->window);
                }
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
    gtk_window_set_title(GTK_WINDOW(main_window), window_title);
    gtk_window_set_default_size(GTK_WINDOW(main_window), win_width, win_height);

    if (win_x >= 0 && win_y >= 0) {
        gtk_window_move(GTK_WINDOW(main_window), win_x, win_y);
    }

    webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(webview));

    if (config_url) {
        webkit_web_view_load_uri(webview, config_url);
    }

    gtk_widget_show_all(main_window);

    return main_window;
}

static void destroy(GtkWidget *window, gpointer data) {
    gtk_main_quit();
}

static gboolean show_window(gpointer data) {
    GtkWidget *window = (GtkWidget *)data;
    gtk_widget_show_all(window);
    return FALSE;
}

static gboolean hide_window(gpointer data) {
    GtkWidget *window = (GtkWidget *)data;
    gtk_widget_hide(window);
    return FALSE;
}

static void parse_args(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"config-file", required_argument, 0, 'c'},
        {"title", required_argument, 0, 't'},
        {"width", required_argument, 0, 'w'},
        {"height", required_argument, 0, 'h'},
        {"x", required_argument, 0, 'x'},
        {"y", required_argument, 0, 'y'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "c:t:w:h:x:y:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c': config_url = g_strdup(optarg); break;
            case 't': window_title = g_strdup(optarg); break;
            case 'w': win_width = atoi(optarg); break;
            case 'h': win_height = atoi(optarg); break;
            case 'x': win_x = atoi(optarg); break;
            case 'y': win_y = atoi(optarg); break;
            default:
                fprintf(stderr, "Usage: %s [--config-file|-c URL] [--title|-t TITLE] [--width|-w N] [--height|-h N] [--x N] [--y N]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}

typedef struct {
    GtkWidget *window;
    char *title;
} AppContext;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    parse_args(argc, argv);

    GtkWidget *main_window = create_yuriwidget_window();

    AppContext *context = g_malloc(sizeof(AppContext));
    context->window = main_window;
    context->title = window_title;

    pthread_t socket_thread;
    pthread_create(&socket_thread, NULL, socket_server_thread, context);

    apply_hyprland_window_settings(main_window);

    g_signal_connect(main_window, "destroy", G_CALLBACK(destroy), NULL);

    gtk_main();

    pthread_join(socket_thread, NULL);

    return 0;
}
