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

int mainFun(int argc, char **argv)
{

//    const int x = atoi(argv[1]);
//    const int y = atoi(argv[2]);
//    const int w = atoi(argv[3]);
//    const int h = atoi(argv[4]);
//    const int zoom = atoi(argv[5]);
//    const char *url = argv[6];

    const int x = 0;
    const int y = 0;
    const int w = 10;
    const int h = 10;
    const int zoom = 100;
    const char *url = argv[1];

    gtk_init(&argc, &argv);


    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "myVandal1");
    gtk_window_set_default_size(GTK_WINDOW(main_window), w, h);
    gtk_window_move(GTK_WINDOW(main_window), x, y);


    gtk_window_set_type_hint(GTK_WINDOW(main_window), GDK_WINDOW_TYPE_HINT_NORMAL);
    gtk_window_set_accept_focus(GTK_WINDOW(main_window), TRUE);
    gtk_window_set_focus_on_map(GTK_WINDOW(main_window), TRUE);


    GtkWidget *webview = webkit_web_view_new();
    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webview), url);
    GdkRGBA trs = {1.0, 1.0, 1.0, 0.0};
    webkit_web_view_set_background_color(WEBKIT_WEB_VIEW(webview), &trs);
    webkit_web_view_set_zoom_level(WEBKIT_WEB_VIEW(webview), zoom / 100.0);

    g_signal_connect(main_window, "destroy", G_CALLBACK(destroy), NULL);

    // Aggiungere il webview alla finestra principale
    gtk_container_add(GTK_CONTAINER(main_window), webview);
//    gtk_widget_set_parent(webview, main_window);
    gtk_widget_show(webview);
    gtk_widget_show(main_window);

    gtk_main();

}

void main(int argc, char **argv)
{
    if (argc != 7)
    {
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
        {
            printf("\t\tmyVandal - Custom version of Vandal widget engine\n");
            printf("myVandal <coor_x> <coor_y> <width> <height> <zoom> <url>\n");
            printf("\t[ ARGUMENTS ]\n");
            printf("<coor_x>\t\t\t(integer) Widget's x coordinate on the screen\n");
            printf("<coor_y>\t\t\t(integer) Widget's y coordinate on the screen\n");
            printf("<width>\t\t\t(integer) Widget's width\n");
            printf("<height>\t\t\t(integer) Widget's height\n");
            printf("<zoom>\t\t\t(integer) Widget's zoom\n");
            printf("<width>\t\t\t(string) (HTML) Widget's config file path\n");

            return 0;
        }
        else
        {
            mainFun(argc, argv);
            return 0;
        }
    }
}
