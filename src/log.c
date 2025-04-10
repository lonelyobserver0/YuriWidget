#include "log.h"
#include <stdio.h>

#define LOG_FILE "/tmp/yuriwidget.log"

void log_message(const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        fprintf(log, "%s\n", message);
        fclose(log);
    }
    fprintf(stderr, "%s\n", message);
}
