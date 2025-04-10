#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <glib.h>

#define SOCKET_PATH "/tmp/yuriwidget.sock"
#define LOG_FILE_PATH "/home/utente/.cache/yuriwidget.log" // Modifica con il tuo username

static GtkWidget *main_window = NULL;
static char *window_title = NULL;
static char *config_file = NULL;
static GHashTable *windows = NULL;
static GMutex mutex;
static FILE *log_file = NULL;

void log_msg(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    if (log_file) {
        va_start(args, format);
        vfprintf(log_file, format, args);
        fflush(log_file);
    }
    va_end(args);
}

static void destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

static void apply_hyprland_window_settings(GtkWidget *window) {
    gtk_widget_realize(window);
    gtk_widget_set_app_paintable(window, TRUE);
    GdkWindow *gdk_window = gtk_widget_get_window(window);

    GdkRGBA transparent;
    gdk_rgba_parse(&transparent, "rgba(0,0,0,0)");
    gdk_window_set_background_rgba(gdk_window, &transparent);

    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    gtk_window_set_accept_focus(GTK_WINDOW(window), FALSE);
}

static GtkWidget* create_yuriwidget_window(const char *title) {
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), title);
    gtk_window_set_default_size(GTK_WINDOW(win), 600, 400);

    WebKitWebView *webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_uri(webview, "https://example.com");

    gtk_container_add(GTK_CONTAINER(win), GTK_WIDGET(webview));
    g_signal_connect(win, "destroy", G_CALLBACK(destroy), NULL);

    apply_hyprland_window_settings(win);
    gtk_widget_show_all(win);

    return win;
}

static void *socket_server_thread(void *arg) {
    unlink(SOCKET_PATH);
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    chmod(SOCKET_PATH, 0666);
    listen(server_fd, 5);

    char buf[256];
    while (1) {
        int client = accept(server_fd, NULL, NULL);
        if (client < 0) continue;
        ssize_t len = read(client, buf, sizeof(buf)-1);
        if (len > 0) {
            buf[len] = '\0';
            char cmd[16], title[240];
            sscanf(buf, "%15s %239[^\n]", cmd, title);
            g_mutex_lock(&mutex);
            GtkWidget *w = g_hash_table_lookup(windows, title);
            if (w) {
                if (strcmp(cmd, "show") == 0) {
                    gtk_widget_show_all(w);
                    log_msg("[yuriwidget] Show: %s\n", title);
                } else if (strcmp(cmd, "hide") == 0) {
                    gtk_widget_hide(w);
                    log_msg("[yuriwidget] Hide: %s\n", title);
                } else {
                    log_msg("[yuriwidget] Unknown command: %s\n", cmd);
                }
            } else {
                log_msg("[yuriwidget] No window with title: %s\n", title);
            }
            g_mutex_unlock(&mutex);
        }
        close(client);
    }
    return NULL;
}

void send_ipc_command(const char *cmd, const char *title) {
    int client = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
        dprintf(client, "%s %s", cmd, title);
    } else {
        fprintf(stderr, "Failed to connect to socket: %s\n", strerror(errno));
    }
    close(client);
}

int main(int argc, char *argv[]) {
    gtk_init(NULL, NULL);
    g_mutex_init(&mutex);

    log_file = fopen(LOG_FILE_PATH, "a");

    if (argc == 3 && (strcmp(argv[1], "show") == 0 || strcmp(argv[1], "hide") == 0)) {
        send_ipc_command(argv[1], argv[2]);
        return 0;
    }

    GOptionEntry entries[] = {
        { "title", 't', 0, G_OPTION_ARG_STRING, &window_title, "Window title", "TITLE" },
        { "config-file", 0, 0, G_OPTION_ARG_STRING, &config_file, "Config file path", "FILE" },
        { "cf", 0, G_OPTION_FLAG_HIDDEN, G_OPTION_ARG_STRING, &config_file, NULL, NULL },
        { NULL }
    };

    GError *error = NULL;
    GOptionContext *context = g_option_context_new("- yuriwidget options");
    g_option_context_add_main_entries(context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_printerr("option parsing failed: %s\n", error->message);
        return 1;
    }

    if (!window_title) window_title = g_strdup("YuriWidget");

    windows = g_hash_table_new(g_str_hash, g_str_equal);
    main_window = create_yuriwidget_window(window_title);
    g_hash_table_insert(windows, window_title, main_window);

    pthread_t tid;
    pthread_create(&tid, NULL, socket_server_thread, NULL);

    gtk_main();

    fclose(log_file);
    return 0;
}
