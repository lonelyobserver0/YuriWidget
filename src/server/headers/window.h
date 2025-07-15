#ifndef WINDOW_H
#define WINDOW_H

#include "config.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <glib.h> // Necessario per guint

// Definizione completa della struttura AppContext
// Questa struct conterrà tutti i dati rilevanti per l'applicazione
// e sarà passata tra le varie funzioni.
typedef struct _AppContext {
    GtkWindow *window;         // La finestra principale dell'applicazione
    WebKitWebView *webview;    // La vista web che mostra il contenuto
    Config *config;            // La configurazione caricata per l'applicazione
    guint timer_id;            // ID del timer per la chiusura automatica (0 se non attivo)
    // Potrebbero essere aggiunti altri membri qui, ad esempio per il server socket
} AppContext;

/**
 * @brief Crea e inizializza il contesto dell'applicazione.
 *
 * Alloca memoria per AppContext e inizializza i suoi membri,
 * inclusa la creazione della finestra GTK e del WebKitWebView.
 *
 * @param cfg La configurazione caricata per l'applicazione.
 * @return Un puntatore alla struttura AppContext allocata dinamicamente in caso di successo,
 * o NULL in caso di errore.
 */
AppContext *create_app_context(Config *cfg);

/**
 * @brief Libera la memoria e le risorse allocate per il contesto dell'applicazione.
 *
 * Distrugge la finestra GTK, libera il WebKitWebView e la configurazione.
 *
 * @param ctx Il puntatore al contesto dell'applicazione da distruggere.
 */
void destroy_app_context(AppContext *ctx);

/**
 * @brief Mostra la finestra principale dell'applicazione.
 *
 * @param ctx Il puntatore al contesto dell'applicazione.
 */
void show_window(AppContext *ctx);

/**
 * @brief Nasconde la finestra principale dell'applicazione.
 *
 * @param ctx Il puntatore al contesto dell'applicazione.
 */
void hide_window(AppContext *ctx);

#endif // WINDOW_H
