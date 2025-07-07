#include "config.h"
#include <json-glib/json-glib.h>
#include <glib.h>
#include <gio/gio.h> // Necessario per GFile, GFileEnumerator, ecc.

/**
 * @brief Ottiene il percorso completo predefinito per il file di configurazione JSON.
 *
 * Costruisce il percorso "$HOME/.config/yuriwidget/config.json".
 *
 * @return Una stringa allocata dinamicamente con il percorso completo.
 * Deve essere liberata con g_free() dopo l'uso.
 */
char *get_default_config_json_path() {
    const char *user_config_dir = g_get_user_config_dir(); // Correzione: const char *
    char *app_config_dir = g_build_filename(user_config_dir, "yuriwidget", NULL);
    char *config_file_path = g_build_filename(app_config_dir, "config.json", NULL);

    // g_free(user_config_dir); // NON liberare, è una stringa gestita da GLib
    g_free(app_config_dir);

    return config_file_path;
}

/**
 * @brief Funzione ausiliaria ricorsiva per cercare un file.
 *
 * @param current_dir Il percorso della directory corrente da cui iniziare la ricerca.
 * @param filename_to_find Il nome del file da cercare.
 * @return Il percorso assoluto del file trovato, o NULL se non trovato.
 * La stringa restituita deve essere liberata con g_free() dopo l'uso.
 */
static char *find_file_recursive(const char *current_dir, const char *filename_to_find) {
    GFile *dir_gfile = g_file_new_for_path(current_dir);
    GFileEnumerator *enumerator = NULL;
    GError *error = NULL;
    char *found_path = NULL;

    enumerator = g_file_enumerate_children(dir_gfile,
                                           "standard::name,standard::file-type", // Correzione: Stringa letterale unica
                                           G_FILE_QUERY_INFO_NONE,
                                           NULL, // GCancellable
                                           &error);

    if (error) {
        // Non stampare errore se la directory non esiste (G_IO_ERROR_NOT_FOUND),
        // potrebbe essere normale in una ricerca ricorsiva (es. una sottocartella vuota o inesistente).
        if (!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND)) {
             g_printerr("Errore durante l'enumerazione della directory '%s': %s\n", current_dir, error->message);
        }
        g_error_free(error);
        g_object_unref(dir_gfile);
        return NULL;
    }

    GFileInfo *file_info;
    while ((file_info = g_file_enumerator_next_file(enumerator, NULL, &error)) != NULL) {
        if (error) {
            g_printerr("Errore durante la lettura del file nella directory '%s': %s\n", current_dir, error->message);
            g_error_free(error);
            break;
        }

        const char *child_name = g_file_info_get_name(file_info);
        GFileType file_type = g_file_info_get_file_type(file_info);

        // Controlla se il nome del file corrisponde
        if (g_strcmp0(child_name, filename_to_find) == 0) {
            found_path = g_build_filename(current_dir, child_name, NULL);
            g_object_unref(file_info); // Libera l'info sul file corrente
            break; // File trovato, esci dal ciclo
        }

        // Se è una directory, cerca ricorsivamente al suo interno
        if (file_type == G_FILE_TYPE_DIRECTORY) {
            char *subdir_path = g_build_filename(current_dir, child_name, NULL);
            found_path = find_file_recursive(subdir_path, filename_to_find);
            g_free(subdir_path); // Libera il percorso della sottodirectory
            if (found_path != NULL) {
                g_object_unref(file_info); // Libera l'info sul file corrente
                break; // File trovato in una sottodirectory, esci dal ciclo
            }
        }
        g_object_unref(file_info); // Libera l'info sul file corrente per il prossimo ciclo
    }

    g_object_unref(enumerator); // Libera l'enumeratore
    g_object_unref(dir_gfile);   // Libera l'oggetto GFile della directory
    return found_path;
}

/**
 * @brief Cerca un file specificato (ad esempio, config.json) ricorsivamente
 * all'interno della directory di configurazione dell'applicazione e delle sue sottocartelle.
 *
 * @param filename_to_find Il nome del file da cercare (es. "config.json").
 * @return Il percorso assoluto del file trovato, o NULL se non trovato.
 * La stringa restituita deve essere liberata con g_free() dopo l'uso.
 */
char *find_file_in_config_dirs(const char *filename_to_find) {
    const char *user_config_dir = g_get_user_config_dir(); // Correzione: const char *
    char *base_app_config_dir = g_build_filename(user_config_dir, "yuriwidget", NULL);
    // g_free(user_config_dir); // NON liberare, è una stringa gestita da GLib

    char *found_path = find_file_recursive(base_app_config_dir, filename_to_find);

    g_free(base_app_config_dir); // Libera la stringa base_app_config_dir
    return found_path;
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

    if (!json_parser_load_from_file(parser, filename, &error)) {
        g_printerr("Errore durante il caricamento del file di configurazione '%s': %s\n", filename, error->message);
        g_error_free(error);
        g_object_unref(parser);
        return NULL;
    }

    JsonNode *root = json_parser_get_root(parser);
    JsonObject *obj = json_node_get_object(root);

    Config *cfg = g_malloc0(sizeof(Config));

    cfg->title = g_strdup(json_object_get_string_member(obj, "title"));

    const char *url = json_object_get_string_member(obj, "url");
    if (g_str_has_prefix(url, "file:")) {
        char *rel_path_segment = g_strdup(url + 5);
        const char *user_config_dir = g_get_user_config_dir(); // Correzione: const char *
        char *resolved_path = g_build_filename(user_config_dir, "yuriwidget", rel_path_segment, NULL);
        cfg->url = g_strdup_printf("file://%s", resolved_path);
        g_free(rel_path_segment);
        g_free(resolved_path);
        // g_free(user_config_dir); // NON liberare, è una stringa gestita da GLib
    } else {
        cfg->url = g_strdup(url);
    }

    cfg->width = json_object_get_int_member(obj, "width");
    cfg->height = json_object_get_int_member(obj, "height");
    cfg->x = json_object_get_int_member(obj, "x");
    cfg->y = json_object_get_int_member(obj, "y");
    cfg->transparent = json_object_get_boolean_member(obj, "transparent");

    g_object_unref(parser);
    return cfg;
}
