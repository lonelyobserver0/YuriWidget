#ifndef WINDOW_H
#define WINDOW_H

#include "config.h"
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <glib.h>

// Definizione completa della struttura AppContext
// Contiene tutti i dati rilevanti per un singolo widget
typedef struct _AppContext {
    GtkWindow *window;
    WebKitWebView *webview;
    Config *config; // Ora punta a Config
    guint timer_id;
    int *open_windows_count; // Puntatore al contatore globale
} AppContext;

/**
 * @brief Crea e inizializza il contesto dell'applicazione per un singolo widget.
 *
 * @param cfg La configurazione specifica per il widget.
 * @param open_windows_count Puntatore a un contatore globale di finestre aperte.
 * @return Un puntatore alla struttura AppContext allocata dinamicamente in caso di successo,
 * o NULL in caso di errore.
 */
AppContext *create_app_context(Config *cfg, int *open_windows_count);

/**
 * @brief Libera la memoria e le risorse allocate per il contesto di un widget.
 *
 * @param ctx Il puntatore al contesto del widget da distruggere.
 */
void destroy_app_context(AppContext *ctx);

/**
 * @brief Mostra la finestra del widget.
 *
 * @param ctx Il puntatore al contesto del widget.
 */
void show_window(AppContext *ctx);

/**
 * @brief Nasconde la finestra del widget.
 *
 * @param ctx Il puntatore al contesto del widget.
 */
void hide_window(AppContext *ctx);

#endif // WINDOW_H
