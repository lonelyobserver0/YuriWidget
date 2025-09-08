#include "window.h"
#include <webkit2/webkit2.h>

// Definiamo una funzione di callback per il timer che chiude la finestra
static gboolean close_window_timer_cb(gpointer data) {
    AppContext *ctx = (AppContext *)data;
    if (ctx && ctx->window) {
        gtk_widget_destroy(GTK_WIDGET(ctx->window));
        // Impostiamo l'ID a 0 per evitare di rimuoverlo una seconda volta in destroy_app_context
        ctx->timer_id = 0;
    }
    return G_SOURCE_REMOVE; // Rimuove il timer dopo l'esecuzione
}

// Funzione di callback per il segnale "destroy"
static void on_window_destroy(GtkWindow *window, AppContext *ctx) {
    g_print("Finestra chiusa. Finestre rimaste: %d\n", (*(ctx->open_windows_count) - 1));
    if (--(*ctx->open_windows_count) == 0) {
        gtk_main_quit();
    }
    // Chiamiamo destroy_app_context per liberare la memoria
    destroy_app_context(ctx);
}

AppContext *create_app_context(Config *cfg, int *open_windows_count) {
    AppContext *ctx = g_malloc0(sizeof(AppContext));
    ctx->config = cfg;
    ctx->open_windows_count = open_windows_count;
    
    // Incrementa il contatore delle finestre aperte
    ++(*open_windows_count);

    ctx->window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(ctx->window, cfg->title);
    gtk_window_set_default_size(ctx->window, cfg->width, cfg->height);
    if (cfg->x >= 0 && cfg->y >= 0) {
        gtk_window_move(ctx->window, cfg->x, cfg->y);
    }

    if (cfg->transparent) {
        gtk_widget_set_app_paintable(GTK_WIDGET(ctx->window), TRUE);
        gtk_widget_set_visual(GTK_WIDGET(ctx->window), gdk_screen_get_rgba_visual(gtk_widget_get_screen(GTK_WIDGET(ctx->window))));
    }

    ctx->webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_uri(ctx->webview, cfg->url);
    gtk_container_add(GTK_CONTAINER(ctx->window), GTK_WIDGET(ctx->webview));

    // Connette il segnale "destroy" alla nuova funzione di callback
    g_signal_connect(ctx->window, "destroy", G_CALLBACK(on_window_destroy), ctx);

    // Imposta il timer se specificato
    if (cfg->timer_seconds > 0) {
        ctx->timer_id = g_timeout_add_seconds(cfg->timer_seconds, close_window_timer_cb, ctx);
    } else {
        ctx->timer_id = 0;
    }

    gtk_widget_show_all(GTK_WIDGET(ctx->window));

    return ctx;
}

void destroy_app_context(AppContext *ctx) {
    if (ctx == NULL) {
        return;
    }
    
    // Rimuove il timer se è attivo
    if (ctx->timer_id != 0) {
        g_source_remove(ctx->timer_id);
    }
    
    // Libera la memoria del contesto, ma non della configurazione
    g_free(ctx);
}

void show_window(AppContext *ctx) {
    gtk_widget_show(GTK_WIDGET(ctx->window));
}

void hide_window(AppContext *ctx) {
    gtk_widget_hide(GTK_WIDGET(ctx->window));
}
