#include "window.h"
#include "args.h"
#include <webkit2/webkit2.h>

GtkWidget *create_yuriwidget_window() {
    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), window_title);
    gtk_window_set_default_size(GTK_WINDOW(main_window), win_width, win_height);

    if (win_x >= 0 && win_y >= 0)
        gtk_window_move(GTK_WINDOW(main_window), win_x, win_y);

    WebKitWebView *webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(main_window), GTK_WIDGET(webview));

    if (config_url)
        webkit_web_view_load_uri(webview, config_url);

    gtk_widget_show_all(main_window);
    return main_window;
}

AppContext *init_app_context(GtkWidget *win, const char *title) {
    AppContext *ctx = g_malloc(sizeof(AppContext));
    ctx->window = win;
    ctx->title = g_strdup(title);
    return ctx;
}

void apply_hyprland_window_settings(GtkWidget *window) {
    GdkScreen *screen = gtk_widget_get_screen(window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual)
        gtk_widget_set_visual(window, visual);
}
