#include "args.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

char *config_url = NULL;
char *window_title = "yuriwidget";
int win_width = 800, win_height = 600, win_x = -1, win_y = -1;

void parse_args(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"config-file", required_argument, 0, 'c'},
        {"title", required_argument, 0, 't'},
        {"width", required_argument, 0, 'w'},
        {"height", required_argument, 0, 'h'},
        {"x", required_argument, 0, 'x'},
        {"y", required_argument, 0, 'y'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "c:t:w:h:x:y:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c': config_url = g_strdup(optarg); break;
            case 't': window_title = g_strdup(optarg); break;
            case 'w': win_width = atoi(optarg); break;
            case 'h': win_height = atoi(optarg); break;
            case 'x': win_x = atoi(optarg); break;
            case 'y': win_y = atoi(optarg); break;
            default:
                fprintf(stderr, "Usage: %s [options]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}
