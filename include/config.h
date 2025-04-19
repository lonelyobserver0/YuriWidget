// === File: include/config.h ===
#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct {
    char *title;
    char *url;
    int width;
    int height;
    int x;
    int y;
    bool transparent;
} YuriConfig;

YuriConfig *load_config_from_file(const char *filename);
void free_config(YuriConfig *config);

#endif
