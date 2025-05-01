#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char *title;
    char *url;
    int width;
    int height;
    int x;
    int y;
    int transparent;
} Config;

Config *config_load_from_file(const char *filename);
void free_config(Config *cfg);

#endif
