#include "headers/clientes.h"
#include "headers/produtos.h"
#include <gtk/gtk.h>
#include <stdlib.h>

Clientes *InicializarClientes();
Produtos *InicializarProdutos();
void InicializarListaClientes(GtkWidget *, GtkBuilder *);
void InicializarListaProdutos(GtkWidget *, GtkBuilder *);

void NovoClientePopup(GtkWidget *, GtkBuilder *);
void NovoClienteSubmit(GtkWidget *, GtkBuilder *);
void ApagarClientePopup(GtkWidget *, GtkBuilder *);

void NovoProdutoPopup(GtkWidget *, GtkBuilder *);

int main(int argc, char **argv){
    gtk_init(&argc, &argv);

    Clientes *lista;
    GtkBuilder *interface;
    const gchar interfaceFilename[] = "interfaces/main.ui";
    GtkWidget *window, *toolbuttonNovoCliente, *toolbuttonNovoProduto, *toolbuttonApagarCliente;
    GError *error = NULL;

    interface = gtk_builder_new();
    if (!gtk_builder_add_from_file(interface, interfaceFilename, &error)){
        printf("Ocorreu um erro: Nao foi possivel inicial a GUI. Erro: %s\n", error->message);
        exit(1);
    }

    InicializarListaClientes(NULL, interface);
    InicializarListaProdutos(NULL, interface);

    window = GTK_WIDGET(gtk_builder_get_object(interface, "window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    toolbuttonNovoCliente = GTK_WIDGET(gtk_builder_get_object(interface, "toolbuttonNovoCliente"));
    g_signal_connect(toolbuttonNovoCliente, "clicked", G_CALLBACK(NovoClientePopup), interface);

    toolbuttonApagarCliente = GTK_WIDGET(gtk_builder_get_object(interface, "toolbuttonApagarCliente"));
    g_signal_connect(toolbuttonApagarCliente, "clicked", G_CALLBACK(ApagarClientePopup), interface);

    toolbuttonNovoProduto = GTK_WIDGET(gtk_builder_get_object(interface, "toolbuttonNovoProduto"));
    g_signal_connect(toolbuttonNovoProduto, "clicked", G_CALLBACK(NovoProdutoPopup), interface);

    gtk_main();

    lista = InicializarClientes();
    salvar_clientes(lista);
}

Clientes *InicializarClientes(){
    static Clientes *lista = NULL;
    if (!lista){
        lista = criar_clientes();
        popular_clientes(lista);
    }
    return (Clientes *) lista;
}

Produtos *InicializarProdutos(){
    static Produtos *lista = NULL;
    if (!lista){
        lista = criar_produtos();
        popular_produtos(lista);
    }

    return (Produtos *) lista;
}

void InicializarListaClientes(GtkWidget *caller, GtkBuilder *interface){
    Clientes *lista = InicializarClientes();
    GtkTreeView *arvore = GTK_TREE_VIEW(gtk_builder_get_object(interface, "tree_view_clientes"));
    GtkListStore *arvoreDados = GTK_LIST_STORE(gtk_tree_view_get_model(arvore));    
    GtkTreeIter iterador;
    char telefone[50], endereco[175];
    int i;

    gtk_list_store_clear(arvoreDados);
    
    for (i = 0; i < lista->tamanho; i++){
        sprintf(telefone, "(%u) %s", lista->clientes[i].telefone.ddd, lista->clientes[i].telefone.telefone);
        sprintf(endereco, "%s %s - %u, %s - %s", lista->clientes[i].endereco.logradouro, lista->clientes[i].endereco.endereco, lista->clientes[i].endereco.casa, lista->clientes[i].endereco.cidade, lista->clientes[i].endereco.estado);

        gtk_list_store_append((GtkListStore *) arvoreDados, &iterador);
        gtk_list_store_set((GtkListStore *) arvoreDados, &iterador, 0, lista->clientes[i].nome, 1, telefone, 2, endereco, -1);
    }
}

void InicializarListaProdutos(GtkWidget *caller, GtkBuilder *interface){
    Produtos *lista = InicializarProdutos();
    GtkTreeView *arvore = GTK_TREE_VIEW(gtk_builder_get_object(interface, "tree_view_produtos"));
    GtkListStore *arvoreDados = GTK_LIST_STORE(gtk_tree_view_get_model(arvore));
    GtkTreeIter iterador;
    char preco[15];
    int i;

    gtk_list_store_clear(arvoreDados);
    for (i = 0; i < lista->tamanho; i++){
        sprintf(preco, "R$ %.2f", lista->produtos[i].preco);

        gtk_list_store_append((GtkListStore *) arvoreDados, &iterador);
        gtk_list_store_set((GtkListStore *) arvoreDados, &iterador, 0, lista->produtos[i].nome, 1, lista->produtos[i].EmEstoque, 2, preco, -1);
    }
}

void NovoClientePopup(GtkWidget *caller, GtkBuilder *mainInterface){
    GtkBuilder *interface;
    const gchar interfaceFilename[] = "interfaces/novocliente.ui";
    GtkWidget *window, *inputSubmit;
    Clientes *lista = InicializarClientes();

    interface = gtk_builder_new();
    if (!gtk_builder_add_from_file(interface, interfaceFilename, NULL)){
        printf("Ocorreu um erro: Nao foi possivel inicial a GUI.\n");
        exit(1);
    }

    window = GTK_WIDGET(gtk_builder_get_object(interface, "dialog"));
    inputSubmit = GTK_WIDGET(gtk_builder_get_object(interface, "inputSubmit"));
    g_signal_connect(inputSubmit, "clicked", G_CALLBACK(NovoClienteSubmit), interface);
    g_signal_connect(window, "destroy", G_CALLBACK(InicializarListaClientes), mainInterface);

    gtk_widget_show(window);
}

void NovoClienteSubmit(GtkWidget *caller, GtkBuilder *interface){
    struct cliente novo_cliente;
    Clientes *lista = InicializarClientes();
    GtkWidget *window, *dialogMessageResult, *inputNome, *inputDDD, *inputTelefone, *inputLogradouro, *inputEndereco, *inputNumero, *inputCidade, *inputEstado;
    
    inputNome = GTK_WIDGET(gtk_builder_get_object(interface, "inputNome"));
    inputDDD = GTK_WIDGET(gtk_builder_get_object(interface, "inputDDD"));
    inputTelefone = GTK_WIDGET(gtk_builder_get_object(interface, "inputTelefone"));
    inputLogradouro = GTK_WIDGET(gtk_builder_get_object(interface, "inputLogradouro"));
    inputEndereco = GTK_WIDGET(gtk_builder_get_object(interface, "inputEndereco"));
    inputNumero = GTK_WIDGET(gtk_builder_get_object(interface, "inputNumero"));
    inputCidade = GTK_WIDGET(gtk_builder_get_object(interface, "inputCidade"));
    inputEstado = GTK_WIDGET(gtk_builder_get_object(interface, "inputEstado"));

    sprintf(novo_cliente.nome, "%s", gtk_entry_get_text(GTK_ENTRY(inputNome)));
    novo_cliente.telefone.ddd = gtk_spin_button_get_value(GTK_SPIN_BUTTON(inputDDD));
    sprintf(novo_cliente.telefone.telefone, "%s", gtk_entry_get_text(GTK_ENTRY(inputTelefone)));
    sprintf(novo_cliente.endereco.logradouro, "%s", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(inputLogradouro)));
    sprintf(novo_cliente.endereco.endereco, "%s", gtk_entry_get_text(GTK_ENTRY(inputEndereco)));
    novo_cliente.endereco.casa = gtk_spin_button_get_value(GTK_SPIN_BUTTON(inputNumero));
    sprintf(novo_cliente.endereco.cidade, "%s", gtk_entry_get_text(GTK_ENTRY(inputCidade)));
    sprintf(novo_cliente.endereco.estado, "%s", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(inputEstado)));
    
    if (strlen(novo_cliente.nome) < 1 || strlen(novo_cliente.telefone.telefone) < 1 || strlen(novo_cliente.endereco.logradouro) < 1 || strlen(novo_cliente.endereco.endereco) < 1 || strlen(novo_cliente.endereco.cidade) < 1 || strlen(novo_cliente.endereco.estado) < 1){
        dialogMessageResult = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Não é permitido campos em branco.");
        gtk_dialog_run(GTK_DIALOG(dialogMessageResult));
        gtk_widget_destroy(dialogMessageResult);
    } else
        if(!adicionar_cliente(lista, novo_cliente)){
            dialogMessageResult = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "Já existe um cliente cadastrado com este nome.");
            gtk_dialog_run(GTK_DIALOG(dialogMessageResult));
            gtk_widget_destroy(dialogMessageResult);
        } else {
            window = GTK_WIDGET(gtk_builder_get_object(interface, "dialog"));
            gtk_widget_destroy(window);
        }
}

void ApagarClientePopup(GtkWidget *caller, GtkBuilder *interface){
    GtkTreeIter *iter;
    GtkTreeView *list;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    Clientes *lista;
    GtkWidget *dialog;
    int result;
    
    lista = InicializarClientes();
    list = GTK_TREE_VIEW(gtk_builder_get_object(interface, "tree_view_clientes"));
    selection = gtk_tree_view_get_selection(list);
    if(!gtk_tree_selection_get_selected(selection, &model, iter))
        return;
    
    gchar *clienteNome;
    gtk_tree_model_get(model, iter, 0, &clienteNome, -1);
    
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_YES_NO, "Apagar cliente?");
    result = gtk_dialog_run(GTK_DIALOG(dialog));
    
    switch (result){
        case GTK_RESPONSE_YES:        
            remover_cliente(lista, clienteNome);
            InicializarListaClientes(NULL, interface);
            break;
        case GTK_RESPONSE_NO:
            printf("No\n");
            break;
        default:
            printf("Unknow\n");
            break;
    }
    gtk_widget_destroy(dialog);
}

void NovoProdutoPopup(GtkWidget *caller, GtkBuilder *mainInterface){
    GtkBuilder *interface;
    const gchar interfaceFilename[] = "interfaces/novoproduto.ui";
    GtkWidget *window, *inputSubmit;

    interface = gtk_builder_new();
    if(!gtk_builder_add_from_file(interface, interfaceFilename, NULL)){
        printf("Ocorreu um erro: Nao foi possivel inicial a GUI.\n");
        exit(1);
    }
    
    window = GTK_WIDGET(gtk_builder_get_object(interface, "dialog"));

    gtk_widget_show(window);
}