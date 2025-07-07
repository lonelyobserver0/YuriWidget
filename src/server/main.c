#include <gtk/gtk.h>
#include <pthread.h>
#include <glib.h> // Necessario per GOptionContext e g_free
#include "window.h" // Assicurati che questo file esista e sia corretto
#include "config.h" // Assicurati che questo file esista e sia corretto (contiene Config e funzioni di caricamento)
#include "socket_server.h" // Assicurati che questo file esista e sia corretto

// Dichiarazione delle funzioni dal file config.c (o config.h se sono lì)
// Queste sono necessarie affinché il compilatore le conosca.
// Se sono già in config.h, non è necessario ridichiararle qui.
extern char *get_default_config_json_path();
extern Config *config_load_from_file(const char *filename);

int main(int argc, char *argv[]) {
    // Variabile per memorizzare il percorso del file di configurazione
    char *config_file_path = NULL;
    GError *error = NULL;

    // Definizione delle opzioni da riga di comando
    GOptionEntry entries[] = {
        { "config-file", 'c', 0, G_OPTION_ARG_STRING, &config_file_path, "Specifica il percorso del file di configurazione JSON", "FILE" },
        { NULL } // Terminatore dell'array
    };

    // Creazione del contesto per l'analisi delle opzioni
    GOptionContext *context = g_option_context_new("- Carica la configurazione del widget");
    g_option_context_add_main_entries(context, entries, NULL);

    // Analisi degli argomenti da riga di comando
    // g_option_context_parse modifica argc e argv per rimuovere le opzioni parsate.
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_printerr("Errore durante l'analisi degli argomenti: %s\n", error->message);
        g_error_free(error);
        g_option_context_free(context);
        return 1; // Termina con errore
    }

    // Se l'opzione --config-file non è stata specificata, usa il percorso predefinito
    if (config_file_path == NULL) {
        config_file_path = get_default_config_json_path();
        g_print("Nessun file di configurazione specificato. Caricamento dal percorso predefinito: %s\n", config_file_path);
    } else {
        g_print("Caricamento dal file di configurazione specificato: %s\n", config_file_path);
    }

    // Inizializza GTK. Deve essere chiamato prima di qualsiasi funzione GTK.
    // g_option_context_parse ha già rimosso le opzioni GLib, quindi argc e argv
    // sono ora pronti per gtk_init.
    gtk_init(&argc, &argv);

    // Carica la configurazione dal file determinato
    Config *cfg = config_load_from_file(config_file_path);
    // Libera il percorso del file di configurazione, non è più necessario dopo il caricamento
    g_free(config_file_path);
    g_option_context_free(context); // Libera il contesto delle opzioni

    if (!cfg) {
        g_printerr("Impossibile caricare la configurazione. Uscita.\n");
        return 1; // Termina con errore se la configurazione non è stata caricata
    }

    // Crea il contesto dell'applicazione
    AppContext *ctx = create_app_context(cfg);
    if (!ctx) {
        g_printerr("Impossibile creare il contesto dell'applicazione. Uscita.\n");
        // Assicurati di liberare cfg se create_app_context fallisce
        // (dipende dall'implementazione di create_app_context se prende possesso di cfg)
        // Per sicurezza, se create_app_context non libera cfg in caso di fallimento:
        // g_free(cfg->title); g_free(cfg->url); g_free(cfg);
        return 1;
    }

    // Avvia il server socket in un thread separato
    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, start_socket_server, ctx) != 0) {
        g_printerr("Errore durante la creazione del thread del server socket.\n");
        destroy_app_context(ctx); // Libera il contesto se il thread non può essere creato
        return 1;
    }

    // Avvia il ciclo principale di GTK. Questo blocco il thread principale.
    gtk_main();

    // Dopo che gtk_main() termina (ad esempio, la finestra viene chiusa),
    // unisciti al thread del server socket per assicurarti che termini correttamente.
    pthread_join(server_thread, NULL);

    // Pulisci le risorse dell'applicazione
    destroy_app_context(ctx);

    return 0; // Uscita pulita
}
