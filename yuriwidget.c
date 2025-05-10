// yuriwidget.c
#include <gtk/gtk.h>
#include <gtk-layer-shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toml.h"

#define CONFIG_PATH "yuriwidget.toml"
#define MAX_LABEL_LEN 256

static void apply_position(GtkWindow *win, int x, int y) {
    gtk_window_move(win, x, y);
}

static void load_config(char *label_text, size_t label_size, int *x, int *y) {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp) return;

    char errbuf[200];
    toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);
    if (!conf) return;

    toml_datum_t label_datum = toml_string_in(conf, "label");
    if (label_datum.ok) {
        snprintf(label_text, label_size, "%s", label_datum.u.s);
        free(label_datum.u.s);
    }

    toml_datum_t x_datum = toml_int_in(conf, "x");
    toml_datum_t y_datum = toml_int_in(conf, "y");
    if (x_datum.ok) *x = (int)x_datum.u.i;
    if (y_datum.ok) *y = (int)y_datum.u.i;

    toml_free(conf);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_window_new();
    gtk_window_set_application(GTK_WINDOW(window), app);

    gtk_layer_init_for_window(GTK_WINDOW(window));
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_OVERLAY);
    gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(window));

    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_hide_on_close(GTK_WINDOW(window), TRUE);

    char label_text[MAX_LABEL_LEN] = "Default";
    int x = 100, y = 100;
    load_config(label_text, sizeof(label_text), &x, &y);

    GtkWidget *label = gtk_label_new(label_text);
    gtk_window_set_child(GTK_WINDOW(window), label);

    gtk_widget_show(window);
    apply_position(GTK_WINDOW(window), x, y);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.example.yuriwidget", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
