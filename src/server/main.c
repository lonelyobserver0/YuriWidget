#include <gtk/gtk.h>
#include <pthread.h>
#include <glib.h> // Necessario per GOptionContext e g_free
#include <string.h> // Necessario per strchr
#include "window.h" // Assicurati che questo file esista e sia corretto
#include "config.h" // Assicurati che questo file esista e sia corretto (contiene Config e funzioni di caricamento)
#include "socket_server.h" // Assicurati che questo file esista e sia corretto

// Dichiarazioni esplicite delle funzioni da config.h per evitare errori di dichiarazione implicita.
// Idealmente, queste dovrebbero essere solo in config.h e main.c dovrebbe solo includere config.h.
// Ma aggiungerle qui garantisce che il compilatore le veda.
extern char *get_default_config_json_path();
extern Config *config_load_from_file(const char *filename);
extern char *find_file_in_config_dirs(const char *filename_to_find);


int main(int argc, char *argv[]) {
    // Variabile per memorizzare il percorso del file di configurazione specificato dall'utente
    char *user_specified_config_arg = NULL; // Questo conterrà l'argomento esatto dell'utente
    char *final_config_file_path = NULL;    // Il percorso assoluto effettivo che useremo
    GError *error = NULL;

    // Definizione delle opzioni da riga di comando
    GOptionEntry entries[] = {
        // L'argomento --config-file accetta ora un nome di file O un percorso relativo
        { "config-file", 'c', 0, G_OPTION_ARG_STRING, &user_specified_config_arg, "Specifica il nome del file di configurazione JSON (es. config.json) o un percorso relativo all'interno di $HOME/.config/yuriwidget (es. sottocartella/config.json)", "PATH_OR_FILENAME" },
        { NULL } // Terminatore dell'array
    };

    // Creazione del contesto per l'analisi delle opzioni
    GOptionContext *context = g_option_context_new("- Carica la configurazione del widget");
    g_option_context_add_main_entries(context, entries, NULL);

    // Analisi degli argomenti da riga di comando
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_printerr("Errore durante l'analisi degli argomenti: %s\n", error->message);
        g_error_free(error);
        g_option_context_free(context);
        return 1;
    }

    // Determina il percorso finale del file di configurazione
    if (user_specified_config_arg != NULL) {
        // Se l'utente ha specificato un argomento, controlla se contiene un separatore di percorso
        // o se inizia con un prefisso di percorso relativo/assoluto
        if (g_str_has_prefix(user_specified_config_arg, "/") || g_str_has_prefix(user_specified_config_arg, "./") || g_str_has_prefix(user_specified_config_arg, "../") || strchr(user_specified_config_arg, G_DIR_SEPARATOR) != NULL) {
            // Sembra essere un percorso (assoluto o relativo con sottocartelle)
            // Costruisci il percorso assoluto relativo alla directory di configurazione dell'app
            const char *user_config_dir = g_get_user_config_dir(); // Correzione: const char *
            char *base_app_config_dir = g_build_filename(user_config_dir, "yuriwidget", NULL);
            // g_free(user_config_dir); // Non liberare, è una stringa gestita da GLib

            final_config_file_path = g_build_filename(base_app_config_dir, user_specified_config_arg, NULL);
            g_free(base_app_config_dir);

            g_print("Tentativo di caricamento diretto del file di configurazione specificato: %s\n", final_config_file_path);

            // Verifica se il file esiste prima di provare a caricarlo
            if (!g_file_test(final_config_file_path, G_FILE_TEST_EXISTS)) {
                g_printerr("Errore: Il file di configurazione specificato '%s' non esiste.\n", final_config_file_path);
                g_free(user_specified_config_arg);
                g_free(final_config_file_path);
                g_option_context_free(context);
                return 1;
            }
        } else {
            // È solo un nome di file (senza sottocartelle), usa la ricerca ricorsiva
            g_print("Ricerca del file di configurazione '%s' in $HOME/.config/yuriwidget e sottocartelle...\n", user_specified_config_arg);
            final_config_file_path = find_file_in_config_dirs(user_specified_config_arg);
            if (final_config_file_path == NULL) {
                g_printerr("Errore: Il file di configurazione '%s' non è stato trovato in $HOME/.config/yuriwidget o nelle sue sottocartelle.\n", user_specified_config_arg);
                g_free(user_specified_config_arg);
                g_option_context_free(context);
                return 1;
            }
            g_print("File di configurazione trovato: %s\n", final_config_file_path);
        }
        g_free(user_specified_config_arg); // Libera la stringa dell'argomento
    } else {
        // Se nessun file è stato specificato, usa il percorso predefinito
        final_config_file_path = get_default_config_json_path();
        g_print("Nessun file di configurazione specificato. Caricamento dal percorso predefinito: %s\n", final_config_file_path);
    }

    // Inizializza GTK. Deve essere chiamato prima di qualsiasi funzione GTK.
    gtk_init(&argc, &argv);

    // Carica la configurazione dal file determinato
    Config *cfg = config_load_from_file(final_config_file_path);
    g_free(final_config_file_path); // Libera il percorso del file di configurazione
    g_option_context_free(context); // Libera il contesto delle opzioni

    if (!cfg) {
        g_printerr("Impossibile caricare la configurazione. Uscita.\n");
        return 1;
    }

    // Crea il contesto dell'applicazione
    AppContext *ctx = create_app_context(cfg);
    if (!ctx) {
        g_printerr("Impossibile creare il contesto dell'applicazione. Uscita.\n");
        // Qui potresti voler liberare cfg se create_app_context non lo fa in caso di fallimento
        // g_free(cfg->title); g_free(cfg->url); g_free(cfg);
        return 1;
    }

    // Avvia il server socket in un thread separato
    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, start_socket_server, ctx) != 0) {
        g_printerr("Errore durante la creazione del thread del server socket.\n");
        destroy_app_context(ctx);
        return 1;
    }

    // Avvia il ciclo principale di GTK. Questo blocco il thread principale.
    gtk_main();

    // Dopo che gtk_main() termina (ad esempio, la finestra viene chiusa),
    // unisciti al thread del server socket per assicurarti che termini correttamente.
    pthread_join(server_thread, NULL);

    // Pulisci le risorse dell'applicazione
    destroy_app_context(ctx);

    return 0;
}
