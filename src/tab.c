#include "tab.h"

// Formata URLs para incluir https:// se necessário
gchar *tab_format_url(const gchar *text) {
    if (g_str_has_prefix(text, "http://") || g_str_has_prefix(text, "https://"))
        return g_strdup(text);
    return g_strdup_printf("https://%s", text);
}

// Quando a URL é digitada e ENTER é pressionado
gboolean on_entry_activate(GtkEntry *entry, gpointer user_data) {
    GtkWidget *notebook = GTK_WIDGET(user_data);
    const gchar *text = gtk_entry_get_text(entry);
    gchar *url = tab_format_url(text);

    GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
    if (!page) {
        g_free(url);
        return TRUE;
    }

    TabData *data = g_object_get_data(G_OBJECT(page), "tab-data");
    webkit_web_view_load_uri(data->webview, url);

    g_free(url);
    return TRUE;
}

// Atualiza a barra de endereço e o título da aba
void on_load_changed(WebKitWebView *webview, WebKitLoadEvent event, gpointer user_data) {
    if (event == WEBKIT_LOAD_COMMITTED) {
        TabData *data = (TabData *)user_data;
        const gchar *uri = webkit_web_view_get_uri(webview);
        gtk_entry_set_text(GTK_ENTRY(data->entry), uri);
    }
}

// Atualiza o título da aba
void on_title_changed(WebKitWebView *webview, GParamSpec *pspec, gpointer page) {
    const gchar *title = webkit_web_view_get_title(webview);
    if (title) {
        GtkWidget *tab_label_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gtk_widget_get_parent(page)), GTK_WIDGET(page));
        GList *children = gtk_container_get_children(GTK_CONTAINER(tab_label_box));
        GtkWidget *label = GTK_WIDGET(g_list_nth_data(children, 0));
        gtk_label_set_text(GTK_LABEL(label), title);
        g_list_free(children);
    }
}

// Fecha a aba atual
void on_tab_close_clicked(GtkButton *btn, gpointer page) {
    GtkWidget *notebook = gtk_widget_get_parent(GTK_WIDGET(page));
    while (notebook && !GTK_IS_NOTEBOOK(notebook))
        notebook = gtk_widget_get_parent(notebook);

    if (notebook && GTK_IS_NOTEBOOK(notebook)) {
        gint index = gtk_notebook_page_num(GTK_NOTEBOOK(notebook), GTK_WIDGET(page));
        if (index != -1)
            gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), index);
        if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) == 0)
            gtk_main_quit(); // Encerra o navegador
    }
}

GtkWidget* tab_create(GtkWidget *notebook, GtkWidget *window) {
    GtkWidget *tab_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    WebKitWebView *webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_box_pack_start(GTK_BOX(tab_box), GTK_WIDGET(webview), TRUE, TRUE, 0);

    // Carrega a página inicial no Google automaticamente
    webkit_web_view_load_uri(webview, "https://www.google.com");
    
    // Obtem a entrada de URL global
    GtkWidget *topbar = g_object_get_data(G_OBJECT(window), "topbar");
    GList *children = gtk_container_get_children(GTK_CONTAINER(topbar));
    GtkWidget *global_entry = GTK_WIDGET(g_list_nth_data(children, 5));
    g_list_free(children);

    TabData *data = g_new(TabData, 1);
    data->webview = webview;
    data->entry = global_entry;
    g_object_set_data_full(G_OBJECT(tab_box), "tab-data", data, g_free);

    g_signal_connect(global_entry, "activate", G_CALLBACK(on_entry_activate), notebook);
    g_signal_connect(webview, "load-changed", G_CALLBACK(on_load_changed), data);
    g_signal_connect(webview, "notify::title", G_CALLBACK(on_title_changed), tab_box);

    GtkWidget *tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *label = gtk_label_new("Nova Aba");
    GtkWidget *close_btn = gtk_button_new_with_label("✕");
    gtk_button_set_relief(GTK_BUTTON(close_btn), GTK_RELIEF_NONE);
    gtk_widget_set_name(close_btn, "tab-close-button");

    gtk_box_pack_start(GTK_BOX(tab_label_box), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tab_label_box), close_btn, FALSE, FALSE, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tab_box, tab_label_box);
    gtk_widget_show_all(tab_box);
    gtk_widget_show_all(tab_label_box);

    g_signal_connect(close_btn, "clicked", G_CALLBACK(on_tab_close_clicked), tab_box);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), -1);

    return tab_box;
}

void tab_go_back(GtkWidget *notebook) {
    GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
    if (!page) return;
    TabData *data = g_object_get_data(G_OBJECT(page), "tab-data");
    if (webkit_web_view_can_go_back(data->webview))
        webkit_web_view_go_back(data->webview);
}

void tab_go_forward(GtkWidget *notebook) {
    GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
    if (!page) return;
    TabData *data = g_object_get_data(G_OBJECT(page), "tab-data");
    if (webkit_web_view_can_go_forward(data->webview))
        webkit_web_view_go_forward(data->webview);
}

void tab_reload(GtkWidget *notebook) {
    GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
    if (!page) return;
    TabData *data = g_object_get_data(G_OBJECT(page), "tab-data");
    if (data && data->webview) {
        webkit_web_view_reload(data->webview);
    }
}