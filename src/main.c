#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <webkit2/webkit2.h>
#include <string.h>
#include <unistd.h>

#define WINDOW_INFO_FILE "/tmp/myVandal_windows.txt"

static void destroy(GtkWidget *widget, gpointer data) {
    // Rimuovi la finestra dal file quando viene distrutta
    FILE *file = fopen(WINDOW_INFO_FILE, "a");
    if (file) {
        fprintf(file, "destroyed: %s\n", (char *)data);
        fclose(file);
    }
    gtk_main_quit();
}

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (event->keyval == GDK_KEY_Escape) {
        gtk_widget_hide(widget); // Nascondi il widget
        return TRUE; // Evento gestito
    }
    return FALSE; // Passa l'evento ad altri gestori
}

void helpMsg(int argc, char **argv);
void showWindow(const char *title);

int main(int argc, char **argv) {
    // Valori default della geometria del widget
    // Non hanno effetto sul risultato poichè sarà hyprland a decidere la posizione e le dimensioni
    const int x = 0;
    const int y = 0;
    const int w = 10;
    const int h = 10;
    const int zoom = 100;
    const char *url = NULL;
    const char *title = NULL; // Inizializza il titolo a NULL

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-config") == 0 && (i + 1) < argc) {
            url = argv[i + 1]; //Prendi il path del file di configurazione dall'argomento successivo
            break;
        }
    }

    // Controlla se il parametro -title è presente
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-title") == 0 && (i + 1) < argc) {
            title = argv[i + 1]; // Prendi il titolo dall'argomento successivo
            break;
        }
    }

    if (title == NULL) {
        fprintf(stderr, "Errore: è necessario specificare un titolo con l'opzione -title.\n");
        return 0;
    }

    // Controlla se il parametro -esc è presente
    gboolean esc_enabled = FALSE;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-esc") == 0) {
            esc_enabled = TRUE;
            break;
        }
    }

    gtk_init(&argc, &argv);

    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), title); // Usa il titolo fornito
    gtk_window_set_default_size(GTK_WINDOW(main_window), w, h);
    gtk_window_move(GTK_WINDOW(main_window), x, y);

    gtk_window_set_type_hint(GTK_WINDOW(main_window), GDK_WINDOW_TYPE_HINT_NORMAL);
    gtk_window_set_accept_focus(GTK_WINDOW(main_window), TRUE);
    gtk_window_set_focus_on_map(GTK_WINDOW(main_window), TRUE);

    GtkWidget *webview = webkit_web_view_new();
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), url);
    GdkRGBA trs = {1.0, 1.0, 1.0, 0.0};
    webkit_web_view_set_background_color(WEBKIT_WEB_VIEW(webview), &trs);

    webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(webview), zoom / 100.0);

    // Aggiungi il gestore per la chiusura della finestra
    g_signal_connect(main_window, "destroy", G_CALLBACK(destroy), (char *)title);

    // Aggiungi il gestore per il tasto "Esc" solo se -esc è presente
    if (esc_enabled) {
        g_signal_connect(main_window, "key-press-event", G_CALLBACK(on_key_press), NULL);
    }

    // Aggiungere il webview alla finestra principale
    gtk_container_add(GTK_CONTAINER(main_window), webview);
    gtk_widget_show(webview);
    gtk_widget_show(main_window);

    // Scrivi le informazioni sulla finestra nel file
    FILE *file = fopen(WINDOW_INFO_FILE, "a");
    if (file) {
        fprintf(file, "created: %s\n", title); // Usa il titolo fornito
        fclose(file);
    }

    gtk_main();

    return 0;
}

void showWindow(const char *title) {
    // Leggi il file delle finestre per trovare la finestra specificata
    FILE *file = fopen(WINDOW_INFO_FILE, "r");
    if (!file) {
        printf("Impossibile aprire il file delle finestre.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, title) != NULL && strstr(line, "created:") != NULL) {
            // Se troviamo la finestra, cerchiamo di mostrare la finestra
            GList *windows = gtk_window_list_toplevels();
            for (GList *l = windows; l != NULL; l = l->next) {
                GtkWidget *window = GTK_WIDGET(l->data);
                if (g_strcmp0(gtk_window_get_title(GTK_WINDOW(window)), title) == 0) {
                    gtk_widget_show(window); // Rendi visibile la finestra
                    fclose(file);
                    return;
                }
            }
        }
    }

    fclose(file);
    printf("Finestra con titolo '%s' non trovata.\n", title);
}

void helpMsg(int argc, char **argv) {
    printf("\t\tmyVandal - Custom version of Vandal widget engine\n");
    printf("\t[ ARGUMENTS ]\n");
    printf("[-esc]\t\t\t\t(optional) Enable hiding the widget with the Esc key\n");
    printf("myVandal show <nome della finestra>\t\tRendi visibile la finestra con il titolo specificato\n");
}
