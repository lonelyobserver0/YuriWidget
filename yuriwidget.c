#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdk.h>
#include "toml.h"

char label_text[256] = "Default Label";
char button_text[256] = "Click Me";
char command[256] = "";

enum PositionMode { POS_ABSOLUTE, POS_PRESET };
enum PositionMode position_mode = POS_PRESET;
int pos_x = -1, pos_y = -1;
char preset[64] = "top-left";

void on_button_clicked(GtkButton *button, gpointer user_data) {
    if (strlen(command) > 0) {
        system(command);
    }
}

void load_config(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Errore: impossibile aprire %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char errbuf[200];
    toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);
    if (!conf) {
        fprintf(stderr, "Errore nel parsing del file TOML: %s\n", errbuf);
        exit(EXIT_FAILURE);
    }

    toml_table_t* widget = toml_table_in(conf, "widget");
    if (widget) {
        toml_rtostr(toml_raw_in(widget, "label"), label_text, sizeof(label_text));
        toml_rtostr(toml_raw_in(widget, "button_text"), button_text, sizeof(button_text));

        char pos_type[64];
        if (toml_rtostr(toml_raw_in(widget, "position_type"), pos_type, sizeof(pos_type)) == 0) {
            if (strcmp(pos_type, "absolute") == 0) {
                position_mode = POS_ABSOLUTE;
                toml_rtoi(toml_raw_in(widget, "x"), &pos_x);
                toml_rtoi(toml_raw_in(widget, "y"), &pos_y);
            } else if (strcmp(pos_type, "preset") == 0) {
                toml_rtostr(toml_raw_in(widget, "position"), preset, sizeof(preset));
                position_mode = POS_PRESET;
            }
        }
    }

    toml_table_t* action = toml_table_in(conf, "action");
    if (action) {
        toml_rtostr(toml_raw_in(action, "command"), command, sizeof(command));
    }

    toml_free(conf);
}

void apply_position(GtkWindow *window) {
    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    if (!monitor) return;

    GdkRectangle geometry;
    gdk_monitor_get_geometry(monitor, &geometry);

    int win_w = 300, win_h = 100;
    int x = 0, y = 0;

    if (position_mode == POS_ABSOLUTE) {
        x = pos_x;
        y = pos_y;
    } else if (position_mode == POS_PRESET) {
        if (strcmp(preset, "top-left") == 0) {
            x = geometry.x + 0;
            y = geometry.y + 0;
        } else if (strcmp(preset, "top-right") == 0) {
            x = geometry.x + geometry.width - win_w;
            y = geometry.y + 0;
        } else if (strcmp(preset, "bottom-left") == 0) {
            x = geometry.x + 0;
            y = geometry.y + geometry.height - win_h;
        } else if (strcmp(preset, "bottom-right") == 0) {
            x = geometry.x + geometry.width - win_w;
            y = geometry.y + geometry.height - win_h;
        } else if (strcmp(preset, "center-left") == 0) {
            x = geometry.x + 0;
            y = geometry.y + (geometry.height - win_h) / 2;
        } else if (strcmp(preset, "center-right") == 0) {
            x = geometry.x + geometry.width - win_w;
            y = geometry.y + (geometry.height - win_h) / 2;
        } else if (strcmp(preset, "center") == 0) {
            x = geometry.x + (geometry.width - win_w) / 2;
            y = geometry.y + (geometry.height - win_h) / 2;
        }
    }

    gtk_window_move(window, x, y);
}

int main(int argc, char *argv[]) {
    const char *config_path = NULL;

    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--config-file") == 0) {
            config_path = argv[i + 1];
            break;
        }
    }

    if (!config_path) {
        fprintf(stderr, "Uso: %s --config-file <percorso_config.toml>\n", argv[0]);
        return EXIT_FAILURE;
    }

    gtk_init();

    load_config(config_path);

    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "yuriwidget");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 100);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *label = gtk_label_new(label_text);
    gtk_box_append(GTK_BOX(box), label);

    GtkWidget *button = gtk_button_new_with_label(button_text);
    gtk_box_append(GTK_BOX(box), button);
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    apply_position(GTK_WINDOW(window));

    gtk_widget_show(window);
    gtk_main();

    return 0;
}
