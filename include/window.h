#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include "config.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *webview;
    Config *config;
} AppContext;

AppContext *create_app_context(Config *cfg);
void destroy_app(AppContext *ctx);
void reload_webview(AppContext *ctx);
void execute_js(AppContext *ctx, const char *js);

#endif
