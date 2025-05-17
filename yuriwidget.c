#include <gtk/gtk.h>
#include <gtk-layer-shell/gtk-layer-shell.h>
#include <toml.h>
#include <stdio.h>
#include <stdlib.h>

#define CONFIG_PATH "/etc/yuriwidget/config.toml"

// Applica la posizione della finestra usando gtk-layer-shell
static void apply_position(GtkWindow *win, int x, int y) {
    gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_LEFT, x);
    gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_TOP, y);
}

// Carica le impostazioni da un file TOML
static void load_config(GtkWidget *label, GtkWindow *window) {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp) {
        g_warning("Impossibile aprire il file di configurazione: %s", CONFIG_PATH);
        return;
    }

    char errbuf[200];
    toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);

    if (!conf) {
        g_warning("Errore nel parsing del file TOML: %s", errbuf);
        return;
    }

    toml_datum_t xval = toml_int_in(conf, "x");
    toml_datum_t yval = toml_int_in(conf, "y");
    toml_datum_t labelval = toml_string_in(conf, "label");

    if (xval.ok && yval.ok) {
        apply_position(window, (int)xval.u.i, (int)yval.u.i);
    } else {
        g_warning("Valori x e y non trovati o invalidi");
    }

    if (labelval.ok) {
        gtk_label_set_text(GTK_LABEL(label), labelval.u.s);
        free(labelval.u.s);  // Deve essere liberato come da documentazione di toml.h
    } else {
        g_warning("Chiave 'label' non trovata nel file di configurazione");
    }

    toml_free(conf);
}

// Funzione chiamata all'avvio dell'applicazione GTK
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "YuriWidget");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // Etichetta centrata
    GtkWidget *label = gtk_label_new(NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_window_set_child(GTK_WINDOW(window), label);

    // Inizializzazione gtk-layer-shell
    gtk_layer_init_for_window(GTK_WINDOW(window));
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
    gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(window));

    // Carica configurazione e presenta la finestra
    load_config(label, GTK_WINDOW(window));
    gtk_window_present(GTK_WINDOW(window));
}

// Entry point
int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.example.yuriwidget", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
