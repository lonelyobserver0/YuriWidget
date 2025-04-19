// === File: include/window.h ===
#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include "config.h"

typedef struct {
    GtkWidget *window;
    WebKitWebView *webview;
    YuriConfig *config;
} AppContext;

AppContext *create_app_context(YuriConfig *config);
void destroy_app(AppContext *ctx);
void reload_webview(AppContext *ctx);
void execute_js(AppContext *ctx, const char *js);
void apply_transparency(GtkWidget *window);

#endif