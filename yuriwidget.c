/* yuriwidget.c */
#define GDK_DISABLE_DEPRECATION_WARNINGS

#include <gtk/gtk.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "deps/tomlc99/toml.h"
#include <gtk-layer-shell.h>

static GtkWidget *window;
static GtkWidget *label;
static int pos_x = 100, pos_y = 100;
static gboolean always_on_top = FALSE;
static gboolean transparent = FALSE;

int load_config(const char *config_path, char *label_text, size_t label_size, int *x, int *y, gboolean *always_on_top, gboolean *transparent) {
    FILE *fp = fopen(config_path, "r");
    if (!fp) {
        fprintf(stderr, "Impossibile aprire il file di configurazione: %s\n", config_path);
        return -1;
    }

    char errbuf[200];
    toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);

    if (!conf) {
        fprintf(stderr, "Errore nel parsing del file TOML: %s\n", errbuf);
        return -1;
    }

    toml_table_t *widget = toml_table_in(conf, "widget");
    if (!widget) {
        fprintf(stderr, "Nessuna sezione [widget] nel file TOML\n");
        toml_free(conf);
        return -1;
    }

    const char *label_raw = toml_raw_in(widget, "label");
    if (label_raw) {
        const char *str = toml_strip_quotes(label_raw);
        strncpy(label_text, str, label_size - 1);
        label_text[label_size - 1] = '\0';
    }

    int64_t x_val, y_val;
    if (toml_rtoi(toml_raw_in(widget, "x"), &x_val) == 0)
        *x = (int)x_val;
    if (toml_rtoi(toml_raw_in(widget, "y"), &y_val) == 0)
        *y = (int)y_val;

    int64_t atop_val, transp_val;
    if (toml_rtoi(toml_raw_in(widget, "always_on_top"), &atop_val) == 0)
        *always_on_top = atop_val ? TRUE : FALSE;
    if (toml_rtoi(toml_raw_in(widget, "transparent"), &transp_val) == 0)
        *transparent = transp_val ? TRUE : FALSE;

    toml_free(conf);
    return 0;
}

static void activate(GtkApplication *app, gpointer user_data) {
    char label_text[256] = "Default text";
    load_config("config.toml", label_text, sizeof(label_text), &pos_x, &pos_y, &always_on_top, &transparent);

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "YuriWidget");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);

    gtk_layer_init_for_window(GTK_WINDOW(window));
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_anchor(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_TOP, pos_y);
    gtk_layer_set_margin(GTK_WINDOW(window), GTK_LAYER_SHELL_EDGE_LEFT, pos_x);

    if (transparent) {
        gtk_widget_set_app_paintable(window, TRUE);
        GdkSurface *surface = gtk_native_get_surface(gtk_widget_get_native(window));
        gdk_surface_set_opaque_region(surface, NULL);
    }

    label = gtk_label_new(label_text);
    gtk_window_set_child(GTK_WINDOW(window), label);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.yuriwidget", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
