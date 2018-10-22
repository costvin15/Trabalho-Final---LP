#include <gtk/gtk.h>
#include "headers/clientes.h"

int main(int argc, char **argv){
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    GError *error = NULL;
    if (gtk_builder_add_from_file(builder, "interfaces/main.ui", &error) == 0){
        return 1;
    }
    
    GtkWidget *mainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    GtkTreeView *tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_view_clientes"));
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(tree));
    GtkTreeIter iter;

    int i;
    Clientes *listaClientes = criar_clientes();
    popular_clientes(listaClientes);

    for (i = 0; i < listaClientes->tamanho; i++){
        char telefone[50];
        char endereco[175];

        sprintf(telefone, "(%u) %s", listaClientes->clientes[i].telefone.ddd, listaClientes->clientes[i].telefone.telefone);
        sprintf(endereco, "%s %s - %u, %s - %s, %s", listaClientes->clientes[i].endereco.logradouro, listaClientes->clientes[i].endereco.endereco, listaClientes->clientes[i].endereco.casa, listaClientes->clientes[i].endereco.cidade, listaClientes->clientes[i].endereco.estado, listaClientes->clientes[i].endereco.pais);

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, listaClientes->clientes[i].nome, 1, telefone, 2, endereco, -1);
    }
    
    tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "tree_view_produtos"));
    store = GTK_LIST_STORE(gtk_tree_view_get_model(tree));

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "Refrigerante Xingu", 1, 10, 2, 5.50, -1);

    g_object_unref(G_OBJECT(builder));
    g_signal_connect(mainWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show(mainWindow);
    gtk_main();
}

/*#include <gtk/gtk.h>
#include "headers/clientes.h"

static void ativarAplicacao(GtkApplication *, gpointer);
static void criarAbas(GtkContainer *);
static GtkWidget *criarClientes();

static void novoCliente();

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

    GtkWidget *ActionBar;
    ActionBar = gtk_action_bar_new();

    GtkWidget *newItemButton;
    newItemButton = gtk_button_new_from_icon_name("document-new", GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_widget_set_tooltip_text(newItemButton, "Novo Cliente");
    gtk_action_bar_pack_start(GTK_ACTION_BAR(ActionBar), newItemButton);
    g_signal_connect(newItemButton, "clicked", G_CALLBACK(novoCliente), NULL);

    GtkWidget *editItemButton;
    editItemButton = gtk_button_new_from_icon_name("applications-office", GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_widget_set_tooltip_text(editItemButton, "Editar Cliente");
    gtk_action_bar_pack_start(GTK_ACTION_BAR(ActionBar), editItemButton);

    GtkWidget *removeItemButton;
    editItemButton = gtk_button_new_from_icon_name("edit-delete", GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_widget_set_tooltip_text(editItemButton, "Remover Cliente");
    gtk_action_bar_pack_start(GTK_ACTION_BAR(ActionBar), editItemButton);

    gtk_box_pack_end(GTK_BOX(box), ActionBar, FALSE, TRUE, 5);

    return box;
}

static void novoCliente(){
    GtkWidget *popup = gtk_dialog_new_with_buttons("Novo Cliente", NULL, GTK_DIALOG_DESTROY_WITH_PARENT, "OK", GTK_RESPONSE_NONE, NULL);
    GtkWidget *popup_content = gtk_dialog_get_content_area(GTK_DIALOG(popup));

    g_signal_connect(popup, "response", G_CALLBACK(gtk_widget_destroy), popup);
    gtk_widget_show_all(popup);
}*/