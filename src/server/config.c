#include "server/config.h"
#include <json-glib/json-glib.h>
#include <glib.h>

Config *config_load_from_file(const char *filename) {
    JsonParser *parser = json_parser_new();
    GError *error = NULL;

    if (!json_parser_load_from_file(parser, filename, &error)) {
        g_printerr("Error loading config: %s\n", error->message);
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
        char *dir = g_path_get_dirname(filename);
        char *rel = g_build_filename(dir, url + 5, NULL);
        cfg->url = g_strdup_printf("file://%s", rel);
        g_free(rel);
        g_free(dir);
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
