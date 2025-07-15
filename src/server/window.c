#include "window.h"
#include <webkit2/webkit2.h>
#include <glib.h> // Necessario per g_timeout_add_seconds, g_source_remove, g_print

// La definizione completa di struct _AppContext è in window.h
// Non è necessario ridefinirla qui, solo usarla.

/**
 * @brief Callback per il timer di chiusura automatica della finestra.
 *
 * Questa funzione viene chiamata quando il timer impostato scade.
 * Distrugge la finestra associata al contesto dell'applicazione.
 *
 * @param user_data Un puntatore al contesto dell'applicazione (AppContext *).
 * @return G_SOURCE_REMOVE per rimuovere il timer dopo l'esecuzione.
 */
static gboolean close_window_timer_cb(gpointer user_data) {
    AppContext *ctx = (AppContext *)user_data;
    g_print("Timer scaduto. Chiusura della finestra.\n");

    // Distrugge la finestra. Questo emetterà il segnale "destroy"
    // a cui è collegato gtk_main_quit() in main.c, terminando l'applicazione.
    gtk_widget_destroy(GTK_WIDGET(ctx->window));

    // Imposta timer_id a 0 per indicare che il timer non è più attivo
    ctx->timer_id = 0;

    return G_SOURCE_REMOVE; // Rimuove questo timer dalla lista dei sorgenti di eventi
}

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
AppContext *create_app_context(Config *cfg) {
    // Alloca memoria per il contesto dell'applicazione e lo inizializza a zero
    AppContext *ctx = g_malloc0(sizeof(AppContext));
    ctx->config = cfg; // Assegna la configurazione caricata
    ctx->timer_id = 0; // Inizializza l'ID del timer a 0 (nessun timer attivo)

    // Crea una nuova finestra GTK di tipo TOPLEVEL
    ctx->window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    // Imposta il titolo della finestra dalla configurazione
    gtk_window_set_title(ctx->window, cfg->title);
    // Imposta le dimensioni predefinite della finestra dalla configurazione
    gtk_window_set_default_size(ctx->window, cfg->width, cfg->height);

    // Se le coordinate x e y sono specificate (non -1), posiziona la finestra
    if (cfg->x >= 0 && cfg->y >= 0) {
        gtk_window_move(ctx->window, cfg->x, cfg->y);
    }

    // Se la configurazione richiede la trasparenza
    if (cfg->transparent) {
        // Rende la finestra "paintable" dall'applicazione
        gtk_widget_set_app_paintable(GTK_WIDGET(ctx->window), TRUE);
        // Imposta il visual per supportare il canale alfa (trasparenza)
        gtk_widget_set_visual(GTK_WIDGET(ctx->window), gdk_screen_get_rgba_visual(gtk_widget_get_screen(GTK_WIDGET(ctx->window))));
    }

    // Crea una nuova vista web WebKit
    ctx->webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    // Carica l'URI specificato nella configurazione nella vista web
    webkit_web_view_load_uri(ctx->webview, cfg->url);
    // Aggiunge la vista web alla finestra
    gtk_container_add(GTK_CONTAINER(ctx->window), GTK_WIDGET(ctx->webview));

    // Connette il segnale "destroy" della finestra alla funzione gtk_main_quit
    // Quando la finestra viene chiusa, l'applicazione GTK terminerà il ciclo principale.
    g_signal_connect(ctx->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Mostra tutti i widget nella finestra (la finestra stessa e la vista web)
    gtk_widget_show_all(GTK_WIDGET(ctx->window));

    // Se un timer è specificato nella configurazione e è maggiore di 0
    if (cfg->timer_seconds > 0) {
        g_print("Impostazione timer per la chiusura automatica tra %d secondi.\n", cfg->timer_seconds);
        // Aggiunge un timeout che chiamerà close_window_timer_cb dopo cfg->timer_seconds secondi
        ctx->timer_id = g_timeout_add_seconds(cfg->timer_seconds, close_window_timer_cb, ctx);
    }

    return ctx; // Restituisce il contesto dell'applicazione creato
}

/**
 * @brief Libera la memoria e le risorse allocate per il contesto dell'applicazione.
 *
 * Distrugge la finestra GTK, libera il WebKitWebView e la configurazione.
 *
 * @param ctx Il puntatore al contesto dell'applicazione da distruggere.
 */
void destroy_app_context(AppContext *ctx) {
    if (ctx == NULL) {
        return;
    }

    // Se un timer è attivo, lo rimuove per prevenire chiamate a callback su memoria liberata
    if (ctx->timer_id != 0) {
        g_print("Rimozione del timer di chiusura automatica.\n");
        g_source_remove(ctx->timer_id);
        ctx->timer_id = 0; // Resetta l'ID del timer
    }

    // Distrugge la finestra GTK (questo distrugge anche i widget figli come il webview)
    if (ctx->window != NULL) {
        gtk_widget_destroy(GTK_WIDGET(ctx->window));
        ctx->window = NULL; // Imposta a NULL per evitare double free/use-after-free
    }

    // Libera la memoria della configurazione usando la funzione dedicata
    if (ctx->config != NULL) {
        config_free(ctx->config);
        ctx->config = NULL; // Imposta a NULL
    }

    // Libera la memoria della struttura AppContext stessa
    g_free(ctx);
}

/**
 * @brief Mostra la finestra principale dell'applicazione.
 *
 * @param ctx Il puntatore al contesto dell'applicazione.
 */
void show_window(AppContext *ctx) {
    if (ctx && ctx->window) {
        gtk_widget_show(GTK_WIDGET(ctx->window));
    }
}

/**
 * @brief Nasconde la finestra principale dell'applicazione.
 *
 * @param ctx Il puntatore al contesto dell'applicazione.
 */
void hide_window(AppContext *ctx) {
    if (ctx && ctx->window) {
        gtk_widget_hide(GTK_WIDGET(ctx->window));
    }
}
