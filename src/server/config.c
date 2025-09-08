#include "config.h"
#include <json-glib/json-glib.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <glib/gstdio.h> // Aggiunta per g_file_test

/**
 * @brief Ottiene il percorso completo predefinito per la directory di configurazione.
 *
 * Costruisce il percorso "$HOME/.config/yuriwidget".
 *
 * @return Una stringa allocata dinamicamente con il percorso completo.
 * Deve essere liberata con g_free() dopo l'uso.
 */
char *get_default_config_dir_path() {
    const char *user_config_dir = g_get_user_config_dir();
    char *app_config_dir = g_build_filename(user_config_dir, "yuriwidget", NULL);
    return app_config_dir;
}

/**
 * @brief Funzione ausiliaria ricorsiva per cercare e caricare file di configurazione.
 *
 * @param current_dir Il percorso della directory corrente da cui iniziare la ricerca.
 * @param configs L'array di puntatori a Config dove aggiungere le configurazioni trovate.
 */
static void find_and_load_recursive(const char *current_dir, GPtrArray *configs) {
    GFile *dir_gfile = g_file_new_for_path(current_dir);
    GFileEnumerator *enumerator = NULL;
    GError *error = NULL;

    enumerator = g_file_enumerate_children(dir_gfile,
                                           "standard::name,standard::file-type",
                                           G_FILE_QUERY_INFO_NONE,
                                           NULL,
                                           &error);

    if (error) {
        if (!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND)) {
             g_printerr("Errore durante l'enumerazione della directory '%s': %s\n", current_dir, error->message);
        }
        g_error_free(error);
        g_object_unref(dir_gfile);
        return;
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
        char *child_path = g_build_filename(current_dir, child_name, NULL);

        if (g_strcmp0(child_name, "config.json") == 0) {
            Config *cfg = config_load_from_file(child_path);
            if (cfg) {
                g_print("Trovato e caricato file di configurazione: %s\n", child_path);
                g_ptr_array_add(configs, cfg);
            }
        }

        if (file_type == G_FILE_TYPE_DIRECTORY) {
            find_and_load_recursive(child_path, configs);
        }

        g_free(child_path);
        g_object_unref(file_info);
    }

    g_object_unref(enumerator);
    g_object_unref(dir_gfile);
}

GPtrArray *load_all_widget_configs() {
    char *base_config_dir = get_default_config_dir_path();

    if (!g_file_test(base_config_dir, G_FILE_TEST_IS_DIR)) {
        g_print("La directory di configurazione predefinita '%s' non esiste.\n", base_config_dir);
        g_free(base_config_dir);
        return g_ptr_array_new_with_free_func((GDestroyNotify)config_free);
    }

    GPtrArray *configs = g_ptr_array_new_with_free_func((GDestroyNotify)config_free);
    find_and_load_recursive(base_config_dir, configs);

    g_free(base_config_dir);
    return configs;
}

static char *find_file_recursive(const char *current_dir, const char *filename_to_find) {
    GFile *dir_gfile = g_file_new_for_path(current_dir);
    GFileEnumerator *enumerator = g_file_enumerate_children(dir_gfile, "standard::name,standard::file-type", G_FILE_QUERY_INFO_NONE, NULL, NULL);
    char *found_path = NULL;

    if (!enumerator) {
        g_object_unref(dir_gfile);
        return NULL;
    }

    GFileInfo *file_info;
    while ((file_info = g_file_enumerator_next_file(enumerator, NULL, NULL)) != NULL) {
        const char *child_name = g_file_info_get_name(file_info);
        GFileType file_type = g_file_info_get_file_type(file_info);
        char *child_path = g_build_filename(current_dir, child_name, NULL);

        if (g_strcmp0(child_name, filename_to_find) == 0) {
            found_path = child_path;
            g_object_unref(file_info);
            break;
        }

        if (file_type == G_FILE_TYPE_DIRECTORY) {
            char *recursive_path = find_file_recursive(child_path, filename_to_find);
            if (recursive_path) {
                found_path = recursive_path;
                g_free(child_path); // Liberiamo il percorso intermedio
                g_object_unref(file_info);
                break;
            }
        }
        g_free(child_path);
        g_object_unref(file_info);
    }
    
    g_object_unref(enumerator);
    g_object_unref(dir_gfile);
    return found_path;
}

char *find_file_in_config_dirs(const char *filename_to_find) {
    char *base_config_dir = get_default_config_dir_path();
    char *found_path = find_file_recursive(base_config_dir, filename_to_find);
    g_free(base_config_dir);
    return found_path;
}

void config_free(Config *cfg) {
    if (cfg == NULL) {
        return;
    }
    g_free(cfg->title);
    g_free(cfg->url);
    g_free(cfg);
}

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
    if (json_node_get_node_type(root) != JSON_NODE_OBJECT) {
        g_printerr("Errore: Il file di configurazione '%s' non è un oggetto JSON.\n", filename);
        g_object_unref(parser);
        return NULL;
    }
    JsonObject *obj = json_node_get_object(root);

    Config *cfg = g_malloc0(sizeof(Config));
    cfg->title = g_strdup(json_object_has_member(obj, "title") ? json_object_get_string_member(obj, "title") : "Yuri Widget");
    cfg->url = g_strdup(json_object_has_member(obj, "url") ? json_object_get_string_member(obj, "url") : "about:blank");
    cfg->width = json_object_has_member(obj, "width") ? json_object_get_int_member(obj, "width") : 400;
    cfg->height = json_object_has_member(obj, "height") ? json_object_get_int_member(obj, "height") : 300;
    cfg->x = json_object_has_member(obj, "x") ? json_object_get_int_member(obj, "x") : -1;
    cfg->y = json_object_has_member(obj, "y") ? json_object_get_int_member(obj, "y") : -1;
    cfg->transparent = json_object_has_member(obj, "transparent") && json_object_get_boolean_member(obj, "transparent");
    int timer_val = json_object_has_member(obj, "timer") ? json_object_get_int_member(obj, "timer") : 0;
    cfg->timer_seconds = (timer_val > 0) ? timer_val : 0;

    if (g_str_has_prefix(cfg->url, "file:")) {
        char *rel_path_segment = g_strdup(cfg->url + 5);
        char *file_dirname = g_path_get_dirname(filename);
        char *file_basename = g_path_get_basename(file_dirname);
        
        // Controlla se il percorso relativo duplica il nome della cartella
        if (file_basename && g_str_has_prefix(rel_path_segment, file_basename)) {
            char *temp = g_strdup(rel_path_segment + strlen(file_basename));
            if (temp && temp[0] == '/') {
                g_free(rel_path_segment);
                rel_path_segment = g_strdup(temp + 1);
            }
            g_free(temp);
        }
        g_free(file_basename);

        char *resolved_path = g_build_filename(file_dirname, rel_path_segment, NULL);
        g_free(cfg->url);
        cfg->url = g_strdup_printf("file://%s", resolved_path);
        g_free(rel_path_segment);
        g_free(file_dirname);
        g_free(resolved_path);
    }
    
    g_object_unref(parser);
    return cfg;
}
