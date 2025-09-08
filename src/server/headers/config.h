#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>

typedef struct {
    char *title;
    char *url;
    int width;
    int height;
    int x;
    int y;
    gboolean transparent;
    int timer_seconds;
} Config;

Config *config_load_from_file(const char *filename);
void config_free(Config *cfg);

char *get_default_config_dir_path();

GPtrArray *load_all_widget_configs();

/**
 * @brief Cerca un file specificato (ad esempio, widgets.json) ricorsivamente
 * all'interno della directory di configurazione dell'applicazione e delle sue sottocartelle.
 *
 * @param filename_to_find Il nome del file da cercare.
 * @return Il percorso assoluto del file trovato, o NULL se non trovato.
 * La stringa restituita deve essere liberata con g_free() dopo l'uso.
 */
char *find_file_in_config_dirs(const char *filename_to_find);

#endif // CONFIG_H
