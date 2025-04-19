// === File: src/window.c ===
#include "window.h"

AppContext *create_app_context(YuriConfig *cfg) {
    AppContext *ctx = g_malloc0(sizeof(AppContext));
    ctx->config = cfg;

    ctx->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(ctx->window), cfg->title);
    gtk_window_set_default_size(GTK_WINDOW(ctx->window), cfg->width, cfg->height);

    if (cfg->x >= 0 && cfg->y >= 0)
        gtk_window_move(GTK_WINDOW(ctx->window), cfg->x, cfg->y);

    ctx->webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(ctx->window), GTK_WIDGET(ctx->webview));
    webkit_web_view_load_uri(ctx->webview, cfg->url);

    if (cfg->transparent)
        apply_transparency(ctx->window);

    gtk_widget_show_all(ctx->window);

    return ctx;
}

void destroy_app(AppContext *ctx) {
    if (!ctx) return;
    free_config(ctx->config);
    g_free(ctx);
}

void reload_webview(AppContext *ctx) {
    webkit_web_view_reload(ctx->webview);
}

void execute_js(AppContext *ctx, const char *js) {
    webkit_web_view_run_javascript(ctx->webview, js, NULL, NULL, NULL);
}

void apply_transparency(GtkWidget *window) {
    gtk_widget_set_app_paintable(window, TRUE);
    gtk_widget_set_opacity(window, 0.9); // Example transparency
}