#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

GtkWidget *notebook;
GtkWidget *window;

typedef struct {
    WebKitWebView *webview;
    GtkWidget *entry;
} TabData;

// Formata URLs para incluir https:// se necessário
gchar *format_url(const gchar *text) {
    if (g_str_has_prefix(text, "http://") || g_str_has_prefix(text, "https://"))
        return g_strdup(text);
    return g_strdup_printf("https://%s", text);
}

// Quando a URL é digitada e ENTER é pressionado
gboolean on_entry_activate(GtkEntry *entry, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(entry);
    gchar *url = format_url(text);

    GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
    TabData *data = g_object_get_data(G_OBJECT(page), "tab-data");
    webkit_web_view_load_uri(data->webview, url);

    g_free(url);
    return TRUE;
}

// Atualiza a barra de endereço e o título da aba
void on_load_changed(WebKitWebView *webview, WebKitLoadEvent event, gpointer user_data) {
    if (event == WEBKIT_LOAD_COMMITTED) {
        const gchar *uri = webkit_web_view_get_uri(webview);
        gtk_entry_set_text(GTK_ENTRY(((TabData *)user_data)->entry), uri);
    }
}

// Atualiza o título da aba
void on_title_changed(WebKitWebView *webview, GParamSpec *pspec, gpointer page) {
    const gchar *title = webkit_web_view_get_title(webview);
    if (title) {
        GtkWidget *tab_label_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook), GTK_WIDGET(page));
        GList *children = gtk_container_get_children(GTK_CONTAINER(tab_label_box));
        GtkWidget *label = GTK_WIDGET(g_list_nth_data(children, 0));
        gtk_label_set_text(GTK_LABEL(label), title);
        g_list_free(children);
    }
}

// Ações dos botões de navegação
void on_back_clicked(GtkEventBox *event_box, gpointer user_data) {
    GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
    TabData *data = g_object_get_data(G_OBJECT(page), "tab-data");
    if (webkit_web_view_can_go_back(data->webview))
        webkit_web_view_go_back(data->webview);
}

void on_forward_clicked(GtkEventBox *event_box, gpointer user_data) {
    GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
    TabData *data = g_object_get_data(G_OBJECT(page), "tab-data");
    if (webkit_web_view_can_go_forward(data->webview))
        webkit_web_view_go_forward(data->webview);
}

// Fecha a aba atual
void on_tab_close_clicked(GtkButton *btn, gpointer page) {
    gint index = gtk_notebook_page_num(GTK_NOTEBOOK(notebook), GTK_WIDGET(page));
    if (index != -1)
        gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), index);
}

// Cria nova aba
void create_new_tab() {
    GtkWidget *tab_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    WebKitWebView *webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_box_pack_start(GTK_BOX(tab_box), GTK_WIDGET(webview), TRUE, TRUE, 0);

    // Obtem a entrada de URL global
    GtkWidget *topbar = g_object_get_data(G_OBJECT(window), "topbar");
    GList *children = gtk_container_get_children(GTK_CONTAINER(topbar));
    GtkWidget *global_entry = GTK_WIDGET(g_list_nth_data(children, 5));
    g_list_free(children);

    TabData *data = g_new(TabData, 1);
    data->webview = webview;
    data->entry = global_entry;
    g_object_set_data_full(G_OBJECT(tab_box), "tab-data", data, g_free);

    g_signal_connect(global_entry, "activate", G_CALLBACK(on_entry_activate), NULL);
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
}

// Botão "+"
void on_new_tab_clicked(GtkEventBox *event_box, gpointer user_data) {
    create_new_tab();
}

// Botões de janela
void on_close_clicked(GtkButton *btn, gpointer data) { gtk_window_close(GTK_WINDOW(window)); }
void on_minimize_clicked(GtkButton *btn, gpointer data) { gtk_window_iconify(GTK_WINDOW(window)); }
void on_maximize_clicked(GtkButton *btn, gpointer data) {
    if (gtk_window_is_maximized(GTK_WINDOW(window)))
        gtk_window_unmaximize(GTK_WINDOW(window));
    else
        gtk_window_maximize(GTK_WINDOW(window));
}

void on_reload_clicked(GtkButton *btn, gpointer user_data) {
    GtkWidget *page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)));
    if (page) {
        TabData *data = g_object_get_data(G_OBJECT(page), "tab-data");
        if (data && data->webview) {
            webkit_web_view_reload(data->webview);
        }
    }
}

// Cria rótulos de botão sem borda: <, >, +
GtkWidget *create_flat_button(const gchar *symbol, GCallback callback) {
    GtkWidget *event_box = gtk_event_box_new();
    GtkWidget *label = gtk_label_new(symbol);
    gtk_container_add(GTK_CONTAINER(event_box), label);
    gtk_widget_set_name(label, "flat-button-label");
    g_signal_connect(event_box, "button-press-event", callback, NULL);
    return event_box;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    gtk_window_set_title(GTK_WINDOW(window), "Tuxfari");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Remove a barra de título e bordas
    //gtk_window_set_decorated(GTK_WINDOW(window), FALSE);


    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Barra de navegação (TOPBAR)
    GtkWidget *topbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), topbar, FALSE, FALSE, 5);
    g_object_set_data(G_OBJECT(window), "topbar", topbar); // usado para atualizar entry

    // Botões estilo Safari
    GtkWidget *ctrl_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(topbar), ctrl_box, FALSE, FALSE, 0);

    GtkWidget *btn_close = gtk_button_new();
    gtk_widget_set_name(btn_close, "close-button");
    gtk_widget_set_margin_start(btn_close, 15); 
    gtk_widget_set_size_request(btn_close, 15, 15);
    gtk_widget_set_valign(btn_close, GTK_ALIGN_CENTER);
    g_signal_connect(btn_close, "clicked", G_CALLBACK(on_close_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(ctrl_box), btn_close, FALSE, FALSE, 0);

    GtkWidget *btn_minimize = gtk_button_new();
    gtk_widget_set_name(btn_minimize, "minimize-button");
    gtk_widget_set_size_request(btn_minimize, 15, 15);
    gtk_widget_set_valign(btn_minimize, GTK_ALIGN_CENTER);
    g_signal_connect(btn_minimize, "clicked", G_CALLBACK(on_minimize_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(ctrl_box), btn_minimize, FALSE, FALSE, 0);

    GtkWidget *btn_maximize = gtk_button_new();
    gtk_widget_set_name(btn_maximize, "maximize-button");
    gtk_widget_set_size_request(btn_maximize, 15, 15);
    gtk_widget_set_valign(btn_maximize, GTK_ALIGN_CENTER);
    g_signal_connect(btn_maximize, "clicked", G_CALLBACK(on_maximize_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(ctrl_box), btn_maximize, FALSE, FALSE, 0);


    // Botões < e >
    GtkWidget *btn_back = create_flat_button("<", G_CALLBACK(on_back_clicked));
    GtkWidget *btn_forward = create_flat_button(">", G_CALLBACK(on_forward_clicked));
    gtk_box_pack_start(GTK_BOX(topbar), btn_back, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(topbar), btn_forward, FALSE, FALSE, 5);

    // Entrada de URL (compartilhada)
    GtkWidget *address_reload_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(address_reload_box, GTK_ALIGN_CENTER); // centraliza o conjunto no espaço disponível
    gtk_widget_set_hexpand(address_reload_box, TRUE);             // para ocupar a largura total

    // Barra de endereço
    GtkWidget *entry_placeholder = gtk_entry_new();
    gtk_widget_set_size_request(entry_placeholder, 600, -1);
    gtk_widget_set_hexpand(entry_placeholder, FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(entry_placeholder), TRUE);
    gtk_box_pack_start(GTK_BOX(address_reload_box), entry_placeholder, FALSE, FALSE, 0);

    // Centraliza o texto
    gtk_entry_set_alignment(GTK_ENTRY(entry_placeholder), 0.5);

    // Botão reload grudado na barra (sem espaçamento)
    GtkWidget *reload_btn = create_flat_button("↻", G_CALLBACK(on_reload_clicked));
    gtk_widget_set_name(reload_btn, "reload-button");
    gtk_box_pack_start(GTK_BOX(address_reload_box), reload_btn, FALSE, FALSE, 0);

    // Agora adiciona o conjunto address_reload_box no topbar ou onde quiser
    gtk_box_pack_start(GTK_BOX(topbar), address_reload_box, TRUE, TRUE, 5);


    // Conecta o sinal Enter à função de navegação
    g_signal_connect(entry_placeholder, "activate", G_CALLBACK(on_entry_activate), NULL);

    // Armazena o entry globalmente para acesso nas abas
    g_object_set_data(G_OBJECT(window), "topbar", topbar);

    // Ação do botão de recarregar
    g_signal_connect(reload_btn, "clicked", G_CALLBACK(on_reload_clicked), NULL);

    // Botão "+"
    GtkWidget *btn_new_tab = create_flat_button("+", G_CALLBACK(on_new_tab_clicked));
    gtk_box_pack_start(GTK_BOX(topbar), btn_new_tab, FALSE, TRUE, 10);

    // Área de abas
    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    // CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
    "#close-button { background: #ff5f57; border-radius: 50%; border: none; padding: 0px; min-width: 5px; min-height: 5px; max-width: 5px; max-height: 5px}\n"
    "#minimize-button { background: #ffbd2e; border-radius: 50%; border: none; padding: 0; min-width: 5px; min-height: 5px; max-width: 5px; max-height: 5px}\n"
    "#maximize-button { background: #28c840; border-radius: 50%; border: none; padding: 0; min-width: 5px; min-height: 5px; max-width: 5px; max-height: 5px}\n"
    "#flat-button-label { font: 16px Sans; padding: 6px; }\n"
    "#tab-close-button { padding: 0; border: none; background: transparent; }",
    -1, NULL);

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    // Primeira aba
    create_new_tab();

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
