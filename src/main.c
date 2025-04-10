#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <webkit2/webkit2.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <glib.h>

#define SOCKET_PATH "/tmp/yuriwidget.sock"

static GList *open_windows = NULL;

static void destroy(GtkWidget *widget, gpointer data) {
    open_windows = g_list_remove(open_windows, widget);
    if (g_list_length(open_windows) == 0) {
        gtk_main_quit();
    }
}

static void apply_hyprland_window_settings(GtkWidget *window) {
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_realize(window);

    GdkWindow *gdk_window = gtk_widget_get_window(window);
    if (gdk_window) {
        GdkRGBA transparent = {0, 0, 0, 0};
        gdk_window_set_background_rgba(gdk_window, &transparent);
    }

    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_UTILITY);
}

static GtkWidget* create_yuriwidget_window(const char *title, const char *url) {
    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), title);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 10, 10);

    apply_hyprland_window_settings(main_window);

    GtkWidget *webview = webkit_web_view_new();
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), url);

    GdkRGBA transparent = {1.0, 1.0, 1.0, 0.0};
    webkit_web_view_set_background_color(WEBKIT_WEB_VIEW(webview), &transparent);
    webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(webview), 1.0);

    gtk_container_add(GTK_CONTAINER(main_window), webview);
    g_signal_connect(main_window, "destroy", G_CALLBACK(destroy), (gpointer)title);
    gtk_widget_show_all(main_window);

    return main_window;
}

static gpointer socket_server_thread(gpointer data) {
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buf[256];

    unlink(SOCKET_PATH);
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return NULL;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        return NULL;
    }

    listen(server_fd, 5);
    chmod(SOCKET_PATH, 0666);

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) continue;

        memset(buf, 0, sizeof(buf));
        read(client_fd, buf, sizeof(buf) - 1);

        char *cmd = strtok(buf, " ");
        char *title = strtok(NULL, "\n");

        if (cmd && title) {
            for (GList *l = open_windows; l != NULL; l = l->next) {
                GtkWidget *w = GTK_WIDGET(l->data);
                const char *win_title = gtk_window_get_title(GTK_WINDOW(w));
                if (g_strcmp0(title, win_title) == 0) {
                    if (strcmp(cmd, "show") == 0)
                        gtk_widget_show(w);
                    else if (strcmp(cmd, "hide") == 0)
                        gtk_widget_hide(w);
                    break;
                }
            }
        }
        close(client_fd);
    }

    close(server_fd);
    return NULL;
}

static void print_help() {
    printf("yuriwidget - Lightweight WebKit GTK Overlay for Hyprland\n");
    printf("Usage:\n");
    printf("  yuriwidget -t <title> -cf <url>\n");
    printf("Options:\n");
    printf("  -t, --title <title>           Set the window title (required)\n");
    printf("  -cf, --config-file <url>      Load web content from URL/file (required)\n");
    printf("  show <title>                  Show window with matching title\n");
    printf("  hide <title>                  Hide window with matching title\n");
}

int main(int argc, char **argv) {
    if (argc >= 3 && (strcmp(argv[1], "show") == 0 || strcmp(argv[1], "hide") == 0)) {
        int sock = socket(AF_UNIX, SOCK_STREAM, 0);
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            perror("connect");
            return 1;
        }

        char message[512];
        snprintf(message, sizeof(message), "%s %s", argv[1], argv[2]);
        write(sock, message, strlen(message));
        close(sock);
        return 0;
    }

    const char *url = NULL;
    const char *title = NULL;

    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-cf") == 0 || strcmp(argv[i], "--config-file") == 0) && i + 1 < argc) {
            url = argv[++i];
        } else if ((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--title") == 0) && i + 1 < argc) {
            title = argv[++i];
        }
    }

    if (!title || !url) {
        print_help();
        return 1;
    }

    gtk_init(&argc, &argv);

    GThread *thread = g_thread_new("socket-server", socket_server_thread, NULL);
    GtkWidget *win = create_yuriwidget_window(title, url);
    open_windows = g_list_append(open_windows, win);

    gtk_main();
    g_thread_join(thread);

    return 0;
}
