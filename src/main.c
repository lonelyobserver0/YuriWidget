#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <webkit2/webkit2.h>
#include <string.h>


static void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

int main(int argc, char **argv)
{
    const int x = atoi(argv[1]);
    const int y = atoi(argv[2]);
    const int w = atoi(argv[3]);
    const int h = atoi(argv[4]);
    const int zoom = atoi(argv[5]);
    const char *url = argv[6];

    gtk_init(&argc, &argv);

    // Creare una finestra normale invece di una finestra popup
    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "vandal");
    gtk_window_set_default_size(GTK_WINDOW(main_window), w, h);
    gtk_window_move(GTK_WINDOW(main_window), x, y);

    // Impostazioni per la finestra
    gtk_window_set_type_hint(GTK_WINDOW(main_window), GDK_WINDOW_TYPE_HINT_NORMAL);
    gtk_window_set_accept_focus(GTK_WINDOW(main_window), TRUE);
    gtk_window_set_focus_on_map(GTK_WINDOW(main_window), TRUE);

    // Creare il webview
    GtkWidget *webview = webkit_web_view_new();
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), url);
    GdkRGBA trs = {1.0, 1.0, 1.0, 0.0};
    webkit_web_view_set_background_color(WEBKIT_WEB_VIEW(webview), &trs);
    webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(webview), zoom / 100.0);

    g_signal_connect(main_window, "destroy", G_CALLBACK(destroy), NULL);

    // Aggiungere il webview alla finestra principale
    gtk_container_add(GTK_CONTAINER(main_window), webview);
    gtk_widget_show(webview);
    gtk_widget_show(main_window);

    gtk_main();

    return 0;
}
