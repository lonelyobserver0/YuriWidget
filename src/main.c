#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <webkit2/webkit2.h>
#include <string.h>
#include <X11/Xlib.h>

static void destroy(GtkWidget *widget, XPointer data)
{
    gtk_main_quit();
}

int main(int argc, char **argv)
{
    if (argc < 7 || argc > 9) {
        printf("Usage: %s x y width height zoom URL [--main-window main_window_id]\n", argv[0]);
        return 1;
    }

    const int x = atoi(argv[1]);
    const int y = atoi(argv[2]);
    const int w = atoi(argv[3]);
    const int h = atoi(argv[4]);
    const int zoom = atoi(argv[5]);
    const char *url = argv[6];
    Window main_window_id = 0;

    // Controlla se è stata fornita l'opzione --main-window
    if (argc == 9 && strcmp(argv[7], "--main-window") == 0) {
        char input_buffer[256];
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            printf("Input ricevuto: %s\n", input_buffer);
            main_window_id = (Window)strtol(input_buffer, NULL, 0);
        }
        else {
            printf("ID non ricevuto");
        }
    }

    gtk_init(&argc, &argv);

    GtkWidget *popup_window = gtk_window_new(GTK_WINDOW_POPUP);
    GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(popup_window));
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    gtk_widget_set_visual(GTK_WIDGET(popup_window), visual);
    gtk_widget_set_app_paintable(GTK_WIDGET(popup_window), TRUE);

    gtk_window_set_title(GTK_WINDOW(popup_window), "vandal");
    gtk_window_set_type_hint(GTK_WINDOW(popup_window), GDK_WINDOW_TYPE_HINT_DESKTOP);
    gtk_window_set_default_size(GTK_WINDOW(popup_window), w, h);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(popup_window), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(popup_window), TRUE);
    gtk_window_set_accept_focus(GTK_WINDOW(popup_window), FALSE);
    gtk_window_set_focus_on_map(GTK_WINDOW(popup_window), FALSE);
    gtk_window_move(GTK_WINDOW(popup_window), x, y);

    gtk_window_stick(GTK_WINDOW(popup_window));

    GtkWidget *webview = webkit_web_view_new();
    webkit_web_view_load_uri((WebKitWebView *)webview, url);
    GdkRGBA trs = {1.0, 1.0, 1.0, 0.0};
    webkit_web_view_set_background_color((WebKitWebView *)webview, &trs);
    webkit_web_view_set_zoom_level((WebKitWebView *)webview, zoom / 100.0);

    g_signal_connect(popup_window, "destroy", G_CALLBACK(destroy), NULL);

    gtk_container_add(GTK_CONTAINER(popup_window), webview);
    gtk_widget_show(webview);
    gtk_widget_show(popup_window);

    // Se è stato fornito un ID di finestra principale, imposta la finestra popup come child
    if (main_window_id) {
        GdkWindow *gdk_main_window = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), main_window_id);
        gtk_window_set_transient_for(GTK_WINDOW(popup_window), GTK_WINDOW(gdk_main_window));
    }

    GdkWindow *gdk_window = gtk_widget_get_window(GTK_WIDGET(popup_window));
    gdk_window_lower(gdk_window);

    gtk_main();

    return 0;
}
