#include <gtk/gtk.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "deps/tomlc99/toml.h"

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
    if (label_raw && toml_rtostr(label_raw, label_text, label_size) != 0) {
        fprintf(stderr, "Errore nella lettura di 'label'\n");
        toml_free(conf);
        return -1;
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

void apply_position(GtkWindow *win, int x, int y) {
    gtk_window_move(win, x, y);
}

static void activate(GtkApplication *app, gpointer user_data) {
    char label_text[256] = "Default text";
    load_config("config.toml", label_text, sizeof(label_text), &pos_x, &pos_y, &always_on_top, &transparent);

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "YuriWidget");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    apply_position(GTK_WINDOW(window), pos_x, pos_y);

    if (always_on_top)
        gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);

    label = gtk_label_new(label_text);
    gtk_window_set_child(GTK_WINDOW(window), label);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.yuriwidget", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
