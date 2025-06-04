#include "ui.h"
#include "tab.h"

static GtkWidget *notebook;
static GtkWidget *window;

GtkWidget* ui_create_flat_button(const gchar *symbol, GCallback callback) {
    GtkWidget *event_box = gtk_event_box_new();
    GtkWidget *label = gtk_label_new(symbol);
    gtk_container_add(GTK_CONTAINER(event_box), label);
    gtk_widget_set_name(label, "flat-button-label");
    g_signal_connect(event_box, "button-press-event", callback, NULL);
    return event_box;
}

static void on_close_clicked(GtkButton *btn, gpointer data) {
    gtk_window_close(GTK_WINDOW(window));
}

static void on_minimize_clicked(GtkButton *btn, gpointer data) {
    gtk_window_iconify(GTK_WINDOW(window));
}

static void on_maximize_clicked(GtkButton *btn, gpointer data) {
    if (gtk_window_is_maximized(GTK_WINDOW(window)))
        gtk_window_unmaximize(GTK_WINDOW(window));
    else
        gtk_window_maximize(GTK_WINDOW(window));
}

static void on_new_tab_clicked(GtkEventBox *event_box, gpointer user_data) {
    tab_create(notebook, window);
}

static void on_back_clicked(GtkEventBox *event_box, gpointer user_data) {
    tab_go_back(notebook);
}

static void on_forward_clicked(GtkEventBox *event_box, gpointer user_data) {
    tab_go_forward(notebook);
}

static void on_reload_clicked(GtkButton *btn, gpointer user_data) {
    tab_reload(notebook);
}

GtkWidget* ui_create_window() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    gtk_window_set_title(GTK_WINDOW(window), "Tuxfari");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    GtkWidget *topbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), topbar, FALSE, FALSE, 5);
    g_object_set_data(G_OBJECT(window), "topbar", topbar);

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

    GtkWidget *btn_back = ui_create_flat_button("<", G_CALLBACK(on_back_clicked));
    GtkWidget *btn_forward = ui_create_flat_button(">", G_CALLBACK(on_forward_clicked));
    gtk_box_pack_start(GTK_BOX(topbar), btn_back, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(topbar), btn_forward, FALSE, FALSE, 5);

    GtkWidget *address_reload_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(address_reload_box, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(address_reload_box, TRUE);

    GtkWidget *entry_placeholder = gtk_entry_new();
    gtk_widget_set_size_request(entry_placeholder, 600, -1);
    gtk_widget_set_hexpand(entry_placeholder, FALSE);
    gtk_editable_set_editable(GTK_EDITABLE(entry_placeholder), TRUE);
    gtk_box_pack_start(GTK_BOX(address_reload_box), entry_placeholder, FALSE, FALSE, 0);
    gtk_entry_set_alignment(GTK_ENTRY(entry_placeholder), 0.5);

    GtkWidget *reload_btn = ui_create_flat_button("↻", G_CALLBACK(on_reload_clicked));
    gtk_widget_set_name(reload_btn, "reload-button");
    gtk_box_pack_start(GTK_BOX(address_reload_box), reload_btn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(topbar), address_reload_box, TRUE, TRUE, 5);

    g_signal_connect(entry_placeholder, "activate", G_CALLBACK(tab_create), NULL);
    g_signal_connect(entry_placeholder, "activate", G_CALLBACK(tab_create), NULL);
    g_signal_connect(entry_placeholder, "activate", G_CALLBACK(tab_create), NULL);
    g_signal_connect(entry_placeholder, "activate", G_CALLBACK(tab_create), NULL);

    g_object_set_data(G_OBJECT(window), "topbar", topbar);

    g_signal_connect(reload_btn, "clicked", G_CALLBACK(on_reload_clicked), NULL);

    GtkWidget *btn_new_tab = ui_create_flat_button("+", G_CALLBACK(on_new_tab_clicked));
    gtk_box_pack_start(GTK_BOX(topbar), btn_new_tab, FALSE, TRUE, 10);

    notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    return window;
}

void ui_setup_css() {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "#close-button { background: #ff5f57; border-radius: 50%; border: none; padding: 0px; min-width: 5px; min-height: 5px; max-width: 5px; max-height: 5px}\n"
        "#minimize-button { background: #ffbd2e; border-radius: 50%; border: none; padding: 0; min-width: 5px; min-height: 5px; max-width: 5px; max-height: 5px}\n"
        "#maximize-button { background: #28c840; border-radius: 50%; border: none; padding: 0; min-width: 5px; min-height: 5px; max-width: 5px; max-height: 5px}\n"
        "#flat-button-label { font: 16px Sans; padding: 6px; }\n"
        "#tab-close-button { padding: 0; border: none; background: transparent; }",
        -1, NULL);

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}
