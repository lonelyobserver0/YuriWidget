#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_main_window_id() {
    FILE *fp;
    char path[1035];
    char *main_window_id = NULL;

    // Esegui il comando hyprctl clients
    fp = popen("hyprctl clients", "r");
    if (fp == NULL) {
        fprintf(stderr, "Errore nell'esecuzione di hyprctl\n");
        return NULL;
    }

    // Leggi l'output riga per riga
    while (fgets(path, sizeof(path), fp) != NULL) {
        printf("Output: %s", path);  // Stampa l'output per il debug
        if (strstr(path, "main") != NULL) {  // Modifica questa condizione in base al tuo criterio
            char *token = strtok(path, " ");
            if (token != NULL) {
                token = strtok(NULL, " ");  // Ottieni il secondo token
                if (token != NULL) {
                    main_window_id = strdup(token);  // Duplica la stringa per restituirla
                    break;
                }
            }
        }
    }

    // Chiudi il processo
    pclose(fp);

    return main_window_id;
}

int main() {
    char *window_id = get_main_window_id();
    if (window_id != NULL) {
        printf("ID della finestra principale: %s\n", window_id);
        free(window_id);  // Libera la memoria allocata
    } else {
        printf("Nessuna finestra principale trovata.\n");
    }
    return 0;
}
