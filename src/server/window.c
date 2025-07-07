#include "src/server/include/window.h"
#include <webkit2/webkit2.h>

struct _AppContext {
    GtkWidget *window;
    WebKitWebView *webview;
    Config *config;
};

AppContext *create_app_context(Config *cfg) {
    AppContext *ctx = g_malloc0(sizeof(AppContext));
    ctx->config = cfg;

    ctx->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ctx->window), cfg->title);
    gtk_window_set_default_size(GTK_WINDOW(ctx->window), cfg->width, cfg->height);
    if (cfg->x >= 0 && cfg->y >= 0) {
        gtk_window_move(GTK_WINDOW(ctx->window), cfg->x, cfg->y);
    }

    if (cfg->transparent) {
        gtk_widget_set_app_paintable(ctx->window, TRUE);
        gtk_widget_set_visual(ctx->window, gdk_screen_get_rgba_visual(gtk_widget_get_screen(ctx->window)));
    }

    ctx->webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_uri(ctx->webview, cfg->url);
    gtk_container_add(GTK_CONTAINER(ctx->window), GTK_WIDGET(ctx->webview));

    g_signal_connect(ctx->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(ctx->window);

    return ctx;
}

void destroy_app_context(AppContext *ctx) {
    gtk_widget_destroy(ctx->window);
    g_free(ctx->config);
    g_free(ctx);
}

void show_window(AppContext *ctx) {
    gtk_widget_show(ctx->window);
}

void hide_window(AppContext *ctx) {
    gtk_widget_hide(ctx->window);
}
