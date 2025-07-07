#pragma once

#include "server/config.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

typedef struct _AppContext AppContext;

AppContext *create_app_context(Config *cfg);
void destroy_app_context(AppContext *ctx);
void show_window(AppContext *ctx);
void hide_window(AppContext *ctx);
