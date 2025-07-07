#pragma once

#include <glib.h> // Necessario per gboolean

typedef struct {
    char *title;
    char *url;
    int width;
    int height;
    int x;
    int y;
    gboolean transparent; // Cambiato da int a gboolean per coerenza con GLib
} Config;

/**
 * @brief Carica la configurazione da un file JSON specificato.
 *
 * @param filename Il percorso del file di configurazione JSON da caricare.
 * @return Un puntatore a una struct Config allocata dinamicamente in caso di successo,
 * o NULL in caso di errore. La struct Config e i suoi membri stringa devono essere
 * liberati con g_free() dopo l'uso.
 */
Config *config_load_from_file(const char *filename);

/**
 * @brief Ottiene il percorso completo predefinito per il file di configurazione JSON.
 *
 * Costruisce il percorso "$HOME/.config/yuriwidget/config.json".
 *
 * @return Una stringa allocata dinamicamente con il percorso completo.
 * Deve essere liberata con g_free() dopo l'uso.
 */
char *get_default_config_json_path();
