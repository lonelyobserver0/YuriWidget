#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h> // Necessario per gboolean

// Definizione della struttura Config
typedef struct {
    char *title;
    char *url;
    int width;
    int height;
    int x;
    int y;
    gboolean transparent;
    int timer_seconds; // Nuovo membro: tempo in secondi per la chiusura automatica
} Config;

// Dichiarazioni delle funzioni pubbliche
char *get_default_config_json_path();
char *find_file_in_config_dirs(const char *filename_to_find); // Aggiunto: necessario per la ricerca ricorsiva dei file di configurazione
Config *config_load_from_file(const char *filename);
void config_free(Config *cfg); // Nuova funzione per liberare la memoria della Config

#endif // CONFIG_H
