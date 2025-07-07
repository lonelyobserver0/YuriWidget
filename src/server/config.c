#include "config.h" // Assicurati che questo file contenga la definizione della struct Config
#include <json-glib/json-glib.h>
#include <glib.h> // Include GLib per g_build_filename, g_get_user_config_dir, g_option_context, ecc.

/**
 * @brief Ottiene il percorso completo predefinito per il file di configurazione JSON.
 *
 * Costruisce il percorso "$HOME/.config/yuriwidget/config.json".
 *
 * @return Una stringa allocata dinamicamente con il percorso completo.
 * Deve essere liberata con g_free() dopo l'uso.
 */
char *get_default_config_json_path() {
    // Ottiene la directory di configurazione dell'utente (es. ~/.config su Linux)
    char *user_config_dir = g_get_user_config_dir();

    // Costruisce il percorso della sottocartella specifica dell'applicazione
    char *app_config_dir = g_build_filename(user_config_dir, "yuriwidget", NULL);

    // Costruisce il percorso completo del file config.json all'interno della sottocartella
    char *config_file_path = g_build_filename(app_config_dir, "config.json", NULL);

    // Libera la memoria allocata per le stringhe intermedie
    g_free(user_config_dir);
    g_free(app_config_dir);

    return config_file_path; // Restituisce il percorso completo
}

/**
 * @brief Carica la configurazione da un file specificato.
 *
 * Questa funzione tenta di parsare il file come JSON usando json-glib.
 *
 * @param filename Il percorso del file di configurazione da caricare.
 * @return Un puntatore a una struct Config allocata dinamicamente in caso di successo,
 * o NULL in caso di errore. La struct Config deve essere liberata
 * (inclusi i suoi membri stringa) con g_free() dopo l'uso.
 */
Config *config_load_from_file(const char *filename) {
    JsonParser *parser = json_parser_new();
    GError *error = NULL;

    // Tenta di caricare e parsare il file come JSON
    if (!json_parser_load_from_file(parser, filename, &error)) {
        g_printerr("Errore durante il caricamento del file di configurazione '%s': %s\n", filename, error->message);
        g_error_free(error); // Libera l'oggetto GError
        g_object_unref(parser); // Rilascia il parser
        return NULL;
    }

    JsonNode *root = json_parser_get_root(parser);
    JsonObject *obj = json_node_get_object(root);

    // Alloca e inizializza la struct Config
    Config *cfg = g_malloc0(sizeof(Config));

    // Estrae i membri dalla configurazione JSON
    // Si assume che questi membri esistano e siano del tipo corretto nel JSON.
    cfg->title = g_strdup(json_object_get_string_member(obj, "title"));

    const char *url = json_object_get_string_member(obj, "url");
    if (g_str_has_prefix(url, "file:")) {
        // Gestione speciale per gli URL che iniziano con "file:",
        // risolvendo il percorso relativo alla directory di configurazione dell'utente.
        // Questo è utile per riferimenti a file locali all'interno della configurazione.
        char *rel_path_segment = g_strdup(url + 5); // Salta "file:"
        char *resolved_path = g_build_filename(g_get_user_config_dir(), "yuriwidget", rel_path_segment, NULL);
        cfg->url = g_strdup_printf("file://%s", resolved_path);
        g_free(rel_path_segment);
        g_free(resolved_path);
    } else {
        cfg->url = g_strdup(url);
    }

    cfg->width = json_object_get_int_member(obj, "width");
    cfg->height = json_object_get_int_member(obj, "height");
    cfg->x = json_object_get_int_member(obj, "x");
    cfg->y = json_object_get_int_member(obj, "y");
    cfg->transparent = json_object_get_boolean_member(obj, "transparent");

    g_object_unref(parser); // Rilascia il parser dopo l'uso
    return cfg;
}
