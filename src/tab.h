#ifndef TAB_H
#define TAB_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

typedef struct {
    WebKitWebView *webview;
    GtkWidget *entry;
} TabData;

GtkWidget* tab_create(GtkWidget *notebook, GtkWidget *window);
void tab_go_back(GtkWidget *notebook);
void tab_go_forward(GtkWidget *notebook);
void tab_reload(GtkWidget *notebook);
void tab_close(GtkWidget *notebook, GtkWidget *page);
gchar* tab_format_url(const gchar *text);

#endif // TAB_H
