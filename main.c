#include <gtk/gtk.h>
#include "headers/clientes.h"

static void printHello(GtkWidget *button, GtkWindow *window){
    GtkWidget *message;
    message = gtk_message_dialog_new(window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Hey, there!");

    gtk_dialog_run(GTK_DIALOG(message));
}

static void activate(GtkApplication *app, gpointer user_data){
    GtkWindow *window;
    GtkWidget *box, *header, *list, *label, *addbutton;
    int i;

    window = GTK_WINDOW(gtk_application_window_new(app));
    header = gtk_header_bar_new();
    addbutton = gtk_button_new_with_label("Adicionar Novo");

    printf("1 -> %p\n", &window);
    
    gtk_header_bar_set_title(GTK_HEADER_BAR(header), "Sistema de Loja NerdZ");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header), "Cadastro de Clientes");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header), TRUE);
    gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header), addbutton);

    gtk_window_set_titlebar(window, header);
    gtk_window_set_default_size(window, 600, 480);
    gtk_widget_show_all(GTK_WIDGET(window));

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), box);

    Clientes *c = criar_clientes();
    popular_clientes(c);

    list = gtk_list_box_new();
    for (i = 0; i < c->tamanho; i++){
        label = gtk_label_new(c->clientes[i].nome);
        gtk_list_box_insert(GTK_LIST_BOX(list), label, i);
    }

    g_signal_connect(addbutton, "clicked", G_CALLBACK(printHello), window);

    gtk_container_add(GTK_CONTAINER(box), list);
    gtk_widget_show_all(GTK_WIDGET(window));
}

int main(int argc, char **argv){
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.viniciuscosta.app", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}