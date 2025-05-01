#include "config.h"
#include <json-glib/json-glib.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Config *config_load_from_file(const char *filename) {
    JsonParser *parser = json_parser_new();
    GError *error = NULL;

    if (!json_parser_load_from_file(parser, filename, &error)) {
        g_printerr("Error loading config file: %s\n", error->message);
        g_error_free(error);
        g_object_unref(parser);
        return NULL;
    }

    JsonNode *root = json_parser_get_root(parser);
    JsonObject *obj = json_node_get_object(root);

    Config *config = g_malloc0(sizeof(Config));

    config->title = g_strdup(json_object_get_string_member(obj, "title"));

    const char *url_value = json_object_get_string_member(obj, "url");
    if (g_str_has_prefix(url_value, "file:")) {
        const char *relative_path = url_value + 5; // skip "file:"
        char *dir = g_path_get_dirname(filename);
        char *full_path = g_build_filename(dir, relative_path, NULL);
        config->url = g_strdup_printf("file://%s", full_path);
        g_free(full_path);
        g_free(dir);
    } else {
        config->url = g_strdup(url_value);
    }

    config->width = json_object_has_member(obj, "width") ? json_object_get_int_member(obj, "width") : 800;
    config->height = json_object_has_member(obj, "height") ? json_object_get_int_member(obj, "height") : 600;
    config->x = json_object_has_member(obj, "x") ? json_object_get_int_member(obj, "x") : -1;
    config->y = json_object_has_member(obj, "y") ? json_object_get_int_member(obj, "y") : -1;
    config->transparent = json_object_has_member(obj, "transparent") && json_object_get_boolean_member(obj, "transparent");

    g_object_unref(parser);
    return config;
}

void free_config(Config *cfg) {
    if (!cfg) return;
    g_free(cfg->title);
    g_free(cfg->url);
    g_free(cfg);
}
