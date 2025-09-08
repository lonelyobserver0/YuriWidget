#include <gtk/gtk.h>
#include <pthread.h>
#include <glib.h>
#include <string.h> // Necessario per strchr
#include "window.h"
#include "config.h"
#include "socket_server.h"

// Dichiarazione di un contatore globale per le finestre aperte
static int open_windows_count = 0;

/**
 * @brief Callback per il segnale "destroy" di una finestra.
 *
 * Decrementa il contatore delle finestre aperte. Se è l'ultima finestra a chiudersi,
 * termina il ciclo principale di GTK.
 *
 * @param widget Il puntatore al widget (finestra) che ha emesso il segnale.
 * @param data Un puntatore al contatore delle finestre aperte.
 */
static void on_window_destroy(GtkWidget *widget, gpointer data) {
    int *count = (int *)data;
    (*count)--;
    g_print("Finestra chiusa. Finestre aperte rimanenti: %d\n", *count);
    if (*count <= 0) {
        g_print("Tutte le finestre sono state chiuse. Uscita dall'applicazione.\n");
        gtk_main_quit();
    }
}

int main(int argc, char *argv[]) {
    // Variabile per memorizzare il percorso del file di configurazione specificato dall'utente
    char *user_specified_config_arg = NULL;
    char *final_config_file_path = NULL;
    GError *error = NULL;

    // Definizione delle opzioni da riga di comando
    GOptionEntry entries[] = {
        { "config-file", 'c', 0, G_OPTION_ARG_STRING, &user_specified_config_arg, "Specifica il nome del file di configurazione JSON (es. widgets.json) o un percorso relativo all'interno di $HOME/.config/yuriwidget", "PATH_OR_FILENAME" },
        { NULL }
    };

    // Creazione del contesto per l'analisi delle opzioni
    GOptionContext *context = g_option_context_new("- Carica la configurazione dei widget");
    g_option_context_add_main_entries(context, entries, NULL);

    // Analisi degli argomenti da riga di comando
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_printerr("Errore durante l'analisi degli argomenti: %s\n", error->message);
        g_error_free(error);
        g_option_context_free(context);
        return 1;
    }
    
    // Inizializza GTK. Deve essere chiamato prima di qualsiasi funzione GTK.
    gtk_init(&argc, &argv);
    
    GPtrArray *configs = NULL;

    // Determina il percorso finale del file di configurazione o esegui la ricerca
    if (user_specified_config_arg != NULL) {
        if (g_str_has_prefix(user_specified_config_arg, "/") || g_str_has_prefix(user_specified_config_arg, "./") || g_str_has_prefix(user_specified_config_arg, "../") || strchr(user_specified_config_arg, G_DIR_SEPARATOR) != NULL) {
            const char *user_config_dir = g_get_user_config_dir();
            char *base_app_config_dir = g_build_filename(user_config_dir, "yuriwidget", NULL);
            final_config_file_path = g_build_filename(base_app_config_dir, user_specified_config_arg, NULL);
            g_free(base_app_config_dir);

            g_print("Tentativo di caricamento diretto del file di configurazione specificato: %s\n", final_config_file_path);

            if (!g_file_test(final_config_file_path, G_FILE_TEST_EXISTS)) {
                g_printerr("Errore: Il file di configurazione specificato '%s' non esiste.\n", final_config_file_path);
                g_free(user_specified_config_arg);
                g_free(final_config_file_path);
                g_option_context_free(context);
                return 1;
            }
            // Carica la singola configurazione da un file specifico
            Config *single_cfg = config_load_from_file(final_config_file_path);
            if (single_cfg) {
                configs = g_ptr_array_new_with_free_func((GDestroyNotify)config_free);
                g_ptr_array_add(configs, single_cfg);
            }
        } else {
            g_print("Ricerca del file di configurazione '%s' in $HOME/.config/yuriwidget e sottocartelle...\n", user_specified_config_arg);
            final_config_file_path = find_file_in_config_dirs(user_specified_config_arg);
            if (final_config_file_path == NULL) {
                g_printerr("Errore: Il file di configurazione '%s' non è stato trovato in $HOME/.config/yuriwidget o nelle sue sottocartelle.\n", user_specified_config_arg);
                g_free(user_specified_config_arg);
                g_option_context_free(context);
                return 1;
            }
            g_print("File di configurazione trovato: %s\n", final_config_file_path);
            Config *single_cfg = config_load_from_file(final_config_file_path);
            if (single_cfg) {
                configs = g_ptr_array_new_with_free_func((GDestroyNotify)config_free);
                g_ptr_array_add(configs, single_cfg);
            }
        }
        g_free(user_specified_config_arg);
        if (final_config_file_path) {
            g_free(final_config_file_path);
        }
    } else {
        // Nessun file specificato, carica tutte le configurazioni trovate
        g_print("Nessun file di configurazione specificato. Caricamento di tutti i 'config.json' trovati in ~/.config/yuriwidget.\n");
        configs = load_all_widget_configs();
    }
    g_option_context_free(context);

    if (!configs || configs->len == 0) {
        g_printerr("Nessuna configurazione widget valida trovata. Uscita.\n");
        if (configs) g_ptr_array_free(configs, TRUE);
        return 1;
    }

    // Imposta il contatore delle finestre aperte
    open_windows_count = configs->len;

    // Crea un contesto e una finestra per ogni widget nella configurazione
    for (guint i = 0; i < configs->len; ++i) {
        Config *widget_cfg = g_ptr_array_index(configs, i);
        AppContext *ctx = create_app_context(widget_cfg, &open_windows_count);

        if (ctx) {
            // Sostituisce la callback di distruzione predefinita con la nostra
            g_signal_connect(GTK_WINDOW(ctx->window), "destroy", G_CALLBACK(on_window_destroy), &open_windows_count);
        } else {
            g_printerr("Impossibile creare il contesto per il widget %d. Saltato.\n", i);
            // Libera la memoria del widget che non può essere creato
            config_free(widget_cfg);
        }
    }

    // Libera l'array di puntatori. I puntatori individuali sono ora gestiti
    // dai rispettivi contesti AppContext.
    g_ptr_array_unref(configs);
    
    // Il server socket deve essere aggiornato per gestire più finestre.
    // L'implementazione attuale è disabilitata per evitare comportamenti indefiniti.
    // Il thread del server non viene più creato in main.
    
    // Avvia il ciclo principale di GTK.
    // Verrà terminato solo quando l'ultima finestra verrà chiusa.
    gtk_main();

    // Il programma termina qui.
    return 0;
}
