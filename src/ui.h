#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>

GtkWidget* ui_create_window();
GtkWidget* ui_create_flat_button(const gchar *symbol, GCallback callback);
void ui_setup_css();

#endif // UI_H
