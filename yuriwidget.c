#include <gtk/gtk.h>
#include <gtk4-layer-shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "toml-c.h"

//==================== LOG ============================
#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"

static void log_info(const char *msg) {
    fprintf(stderr, ANSI_BLUE "[INFO] %s" ANSI_RESET "\n", msg);
}

static void log_success(const char *msg) {
    fprintf(stderr, ANSI_GREEN "[OK] %s" ANSI_RESET "\n", msg);
}

static void log_warn(const char *msg) {
    fprintf(stderr, ANSI_YELLOW "[WARN] %s" ANSI_RESET "\n", msg);
}

static void log_error(const char *msg) {
    fprintf(stderr, ANSI_RED "[ERROR] %s" ANSI_RESET "\n", msg);
}

//======================================================

#define CONFIG_DIR "/etc/yuriwidget"
#define CONFIG_PATH CONFIG_DIR "/config.toml"
#define STYLE_SCSS CONFIG_DIR "/config.scss"
#define STYLE_CSS  CONFIG_DIR "/config.css"

static void compile_scss() {
    struct stat scss_stat, css_stat;

    if (stat(STYLE_SCSS, &scss_stat) != 0) {
        g_warning("File SCSS non trovato: %s", STYLE_SCSS);
        return;
    }

    if (stat(STYLE_CSS, &css_stat) != 0 || scss_stat.st_mtime > css_stat.st_mtime) {
        g_message("Compilazione di %s in %s", STYLE_SCSS, STYLE_CSS);
        int ret = system("sassc " STYLE_SCSS " " STYLE_CSS);
        if (ret != 0)
            g_warning("Errore nella compilazione SCSS (sassc)");
    }
}

static void load_style(GtkWidget *widget) {
    compile_scss();

    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;

    if (!g_file_test(STYLE_CSS, G_FILE_TEST_EXISTS)) {
        g_warning("Style file not found: %s", STYLE_CSS);
        return;
    }

    gtk_css_provider_load_from_path(provider, STYLE_CSS, &error);
    if (error) {
        g_warning("Failed to load style: %s", error->message);
        g_error_free(error);
        return;
    }

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(provider);
}

static void apply_position(GtkWindow *win, int x, int y) {
    gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_LEFT, TRUE);
    gtk_layer_set_anchor(win, GTK_LAYER_SHELL_EDGE_TOP, TRUE);
    gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_LEFT, x);
    gtk_layer_set_margin(win, GTK_LAYER_SHELL_EDGE_TOP, y);
}

static void load_config(GtkWidget *label, GtkWindow *window) {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp) {
        g_warning("Could not open config file: %s", CONFIG_PATH);
        return;
    }

    char *buf = NULL;
    size_t len;
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buf = malloc(len + 1);
    fread(buf, 1, len, fp);
    buf[len] = '\0';
    fclose(fp);

    toml_table_t conf = toml_parse(buf);
    free(buf);
    if (!conf.ok) {
        g_warning("TOML parse error: %s", conf.errmsg);
        return;
    }

    int x = toml_int(conf, "x", 0);
    int y = toml_int(conf, "y", 0);
    const char *text = toml_string(conf, "label", "");

    apply_position(window, x, y);
    gtk_label_set_text(GTK_LABEL(label), text);

    toml_free(conf);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "YuriWidget");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // Aggiunta classe CSS alla finestra
    gtk_widget_add_css_class(window, "yuri-window");

    GtkWidget *label = gtk_label_new(NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

    // Aggiunta classe e ID CSS all’etichetta
    gtk_widget_add_css_class(label, "yuri-label");
    gtk_widget_set_name(label, "main-label");

    gtk_window_set_child(GTK_WINDOW(window), label);

    gtk_layer_init_for_window(GTK_WINDOW(window));
    gtk_layer_set_layer(GTK_WINDOW(window), GTK_LAYER_SHELL_LAYER_TOP);
    gtk_layer_set_keyboard_mode(GTK_WINDOW(window), GTK_LAYER_SHELL_KEYBOARD_MODE_NONE);
    gtk_layer_auto_exclusive_zone_enable(GTK_WINDOW(window));

    load_style(window);
    load_config(label, GTK_WINDOW(window));

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.example.yuriwidget", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
