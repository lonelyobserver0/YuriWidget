#pragma once

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
