#include <gtk/gtk.h>
#include "ui.h"
#include "tab.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = ui_create_window();
    ui_setup_css();

    // Cria a primeira aba
    GtkWidget *notebook = g_object_get_data(G_OBJECT(window), "notebook");
    if (!notebook) {
        // Buscar notebook diretamente da interface
        GList *children = gtk_container_get_children(GTK_CONTAINER(gtk_bin_get_child(GTK_BIN(window))));
        for (GList *l = children; l != NULL; l = l->next) {
            if (GTK_IS_NOTEBOOK(l->data)) {
                notebook = GTK_WIDGET(l->data);
                g_object_set_data(G_OBJECT(window), "notebook", notebook);
                break;
            }
        }
        g_list_free(children);
    }
    tab_create(notebook, window);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
