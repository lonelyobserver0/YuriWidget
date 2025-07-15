#include "config.h"
#include <json-glib/json-glib.h>
#include <glib.h>
#include <gio/gio.h>

/**
 * @brief Ottiene il percorso completo predefinito per il file di configurazione JSON.
 *
 * Costruisce il percorso "$HOME/.config/yuriwidget/config.json".
 *
 * @return Una stringa allocata dinamicamente con il percorso completo.
 * Deve essere liberata con g_free() dopo l'uso.
 */
char *get_default_config_json_path() {
    const char *user_config_dir = g_get_user_config_dir();
    char *app_config_dir = g_build_filename(user_config_dir, "yuriwidget", NULL);
    char *config_file_path = g_build_filename(app_config_dir, "config.json", NULL);

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
                                           "standard::name,standard::file-type",
                                           G_FILE_QUERY_INFO_NONE,
                                           NULL, // GCancellable
                                           &error);

    if (error) {
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

        if (g_strcmp0(child_name, filename_to_find) == 0) {
            found_path = g_build_filename(current_dir, child_name, NULL);
            g_object_unref(file_info);
            break;
        }

        if (file_type == G_FILE_TYPE_DIRECTORY) {
            char *subdir_path = g_build_filename(current_dir, child_name, NULL);
            found_path = find_file_recursive(subdir_path, filename_to_find);
            g_free(subdir_path);
            if (found_path != NULL) {
                g_object_unref(file_info);
                break;
            }
        }
        g_object_unref(file_info);
    }

    g_object_unref(enumerator);
    g_object_unref(dir_gfile);
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
    const char *user_config_dir = g_get_user_config_dir();
    char *base_app_config_dir = g_build_filename(user_config_dir, "yuriwidget", NULL);

    char *found_path = find_file_recursive(base_app_config_dir, filename_to_find);

    g_free(base_app_config_dir);
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

    // Inizializza timer_seconds a 0 (nessun timer) per default
    cfg->timer_seconds = 0;

    // Estrae i membri JSON e li assegna alla struct Config
    // Assicurati che i membri esistano prima di tentare di accedervi
    if (json_object_has_member(obj, "title")) {
        cfg->title = g_strdup(json_object_get_string_member(obj, "title"));
    } else {
        cfg->title = g_strdup("YuriWidget"); // Valore di default
    }

    if (json_object_has_member(obj, "url")) {
        const char *url = json_object_get_string_member(obj, "url");
        if (g_str_has_prefix(url, "file:")) {
            char *rel_path_segment = g_strdup(url + 5);
            const char *user_config_dir = g_get_user_config_dir();
            char *resolved_path = g_build_filename(user_config_dir, "yuriwidget", rel_path_segment, NULL);
            cfg->url = g_strdup_printf("file://%s", resolved_path);
            g_free(rel_path_segment);
            g_free(resolved_path);
        } else {
            cfg->url = g_strdup(url);
        }
    } else {
        cfg->url = g_strdup("about:blank"); // Valore di default
    }

    if (json_object_has_member(obj, "width")) {
        cfg->width = json_object_get_int_member(obj, "width");
    } else {
        cfg->width = 400; // Valore di default
    }

    if (json_object_has_member(obj, "height")) {
        cfg->height = json_object_get_int_member(obj, "height");
    } else {
        cfg->height = 300; // Valore di default
    }

    if (json_object_has_member(obj, "x")) {
        cfg->x = json_object_get_int_member(obj, "x");
    } else {
        cfg->x = -1; // Valore di default (posizionamento automatico)
    }

    if (json_object_has_member(obj, "y")) {
        cfg->y = json_object_get_int_member(obj, "y");
    } else {
        cfg->y = -1; // Valore di default (posizionamento automatico)
    }

    if (json_object_has_member(obj, "transparent")) {
        cfg->transparent = json_object_get_boolean_member(obj, "transparent");
    } else {
        cfg->transparent = FALSE; // Valore di default
    }

    // Leggi il nuovo campo 'timer'
    if (json_object_has_member(obj, "timer")) {
        JsonNode *timer_node = json_object_get_member(obj, "timer");
        if (json_node_get_value_type(timer_node) == G_TYPE_INT) {
            cfg->timer_seconds = json_object_get_int_member(obj, "timer");
            if (cfg->timer_seconds < 0) {
                cfg->timer_seconds = 0; // Assicurati che il timer non sia negativo
                g_printerr("Avviso: Il valore 'timer' nel file di configurazione è negativo. Impostato a 0.\n");
            }
        } else {
            g_printerr("Avviso: Il membro 'timer' nel file di configurazione non è un intero. Ignorato.\n");
        }
    }

    g_object_unref(parser);
    return cfg;
}

/**
 * @brief Libera la memoria allocata per una struttura Config.
 *
 * @param cfg Il puntatore alla struttura Config da liberare.
 */
void config_free(Config *cfg) {
    if (cfg == NULL) {
        return;
    }
    g_free(cfg->title);
    g_free(cfg->url);
    g_free(cfg);
}
