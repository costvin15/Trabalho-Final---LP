#include <gtk/gtk.h>
#include "headers/clientes.h"

static void ativarAplicacao(GtkApplication *, gpointer);
static void criarAbas(GtkContainer *);
static GtkWidget *criarClientes();

int main(int argc, char **argv){
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.viniciuscosta.trabalhofinal", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(ativarAplicacao), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

static void ativarAplicacao(GtkApplication *app, gpointer user_data){
    GtkWidget *janela;
    janela = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(janela), 750, 600);

    GtkWidget *cabecalho;
    cabecalho = gtk_header_bar_new();
    gtk_header_bar_set_title(GTK_HEADER_BAR(cabecalho), "NerdZ");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(cabecalho), "Sistema de Loja");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(cabecalho), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(janela), cabecalho);

    criarAbas(GTK_CONTAINER(janela));

    gtk_widget_show_all(GTK_WIDGET(janela));
}

static void criarAbas(GtkContainer *container){
    GtkWidget *abas;
    abas = gtk_notebook_new();
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(abas), TRUE);

    GtkWidget *clientesLabel;
    clientesLabel = gtk_label_new("Clientes");
    gtk_notebook_append_page(GTK_NOTEBOOK(abas), criarClientes(), clientesLabel);

    
    GtkWidget *produtosLabel;
    produtosLabel = gtk_label_new("Produtos");
    gtk_notebook_append_page(GTK_NOTEBOOK(abas), criarClientes(), produtosLabel);

    gtk_container_add(container, abas);
}

static GtkWidget *criarClientes(){
    Clientes *c;
    c = criar_clientes();
    popular_clientes(c);

    GtkWidget *box;
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *listView;
    listView = gtk_tree_view_new();
    GtkCellRenderer *renderer;
    renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(listView), -1, "Nome", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(listView), -1, "Telefone", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(listView), -1, "Endereço", renderer, "text", 2, NULL);

    GtkListStore *store;
    GtkTreeIter iter;

    store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    int i;
    for (i = 0; i < c->tamanho; i++){
        char telefone[50];
        char endereco[175];

        sprintf(telefone, "(%u) %s", c->clientes[i].telefone.ddd, c->clientes[i].telefone.telefone);
        sprintf(endereco, "%s %s - %u, %s - %s, %s", c->clientes[i].endereco.logradouro, c->clientes[i].endereco.endereco, c->clientes[i].endereco.casa, c->clientes[i].endereco.cidade, c->clientes[i].endereco.estado, c->clientes[i].endereco.pais);

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, c->clientes[i].nome, 1, telefone, 2, endereco, -1);
    }

    GtkTreeModel *model;
    model = GTK_TREE_MODEL(store);
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(listView), model);

    gtk_container_add(GTK_CONTAINER(box), listView);
    return box;
}