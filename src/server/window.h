#pragma once
#include <gtk/gtk.h>

typedef struct {
    GtkWidget *window;
    char *title;
} AppContext;

GtkWidget *create_yuriwidget_window(void);
AppContext *init_app_context(GtkWidget *win, const char *title);
void apply_hyprland_window_settings(GtkWidget *window);
