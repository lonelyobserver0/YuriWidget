#include <gtk/gtk.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <toml.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CONFIG_PATH "/etc/yuriwidget/config.toml"
#define MAX_LABEL_LEN 256

static void apply_position(GtkWindow *win, int x, int y) {
    gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_LEFT, x);
    gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_TOP, y);
}

static void load_config(GtkWidget *label, GtkWindow *window) {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp) return;

    char errbuf[200];
    toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);
    if (!conf) return;

    toml_datum_t xval = toml_int_in(conf, "x");
    toml_datum_t yval = toml_int_in(conf, "y");
    toml_datum_t labelval = toml_string_in(conf, "label");

    if (xval.ok && yval.ok) {
        apply_position(window, (int)xval.u.i, (int)yval.u.i);
    }
    if (labelval.ok) {
        gtk_label_set_text(GTK_LABEL(label), labelval.u.s);
        free(labelval.u.s);
    }

    toml_free(conf);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_window_new();
    gtk_window_set_application(GTK_WINDOW(window), app);
    gtk_window_set_title(GTK_WINDOW(window), "YuriWidget");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_widget_set_halign(window, GTK_ALIGN_START);
    gtk_widget_set_valign(window, GTK_ALIGN_START);

    GtkWidget *label = gtk_label_new("");
    gtk_window_set_child(GTK_WINDOW(window), label);

    gtk_layer_init_for_window(GTK_WINDOW(window));
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
    gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(window));

    gtk_window_present(GTK_WINDOW(window));

    load_config(label, GTK_WINDOW(window));
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
