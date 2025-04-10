#include <gtk/gtk.h>
#include <webkit/webkit.h>

static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "myVandal");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // WebView
    WebKitWebView *web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    webkit_web_view_load_uri(web_view, "https://example.com");

    // Contenitore
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), box);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(web_view));

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.my.vandal", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
