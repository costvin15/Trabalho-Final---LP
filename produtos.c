//  ;=====================================
//  ;  Title: produtos.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 09/11/18
//  ;=====================================

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "headers/produtos.h"

static Produtos *PRODUTOS_LISTA = NULL;

void produtos_gui(GtkBuilder *, Produtos *);
void produtos_aba(GtkBuilder *, Produtos *);
void recarregar_aba_produtos(GtkBuilder *, GtkBuilder *);
void mensagem_simples_produtos(GtkWindow *, gchar *, gchar *);

void produtos_botao(GtkBuilder *);

void novo_produto_dialog(GtkWidget *, GtkBuilder *);
void novo_produto_callback(GtkWidget *, GtkBuilder *);

void editar_produto_dialog(GtkWidget *, GtkBuilder *);
void editar_produto_callback(GtkWidget *, void **);

void remover_produto_dialog(GtkWidget *, GtkBuilder *);
void remover_produto_callback(GtkWidget *, int, int *);

void produtos_gui(GtkBuilder *interface, Produtos *produtos){
    PRODUTOS_LISTA = produtos;
    produtos_aba(interface, produtos);
}

void produtos_aba(GtkBuilder *interface, Produtos *produtos){
    static GtkBuilder *mInterface = NULL;
    if (interface == NULL){
        interface = mInterface;
        produtos = PRODUTOS_LISTA;
    }
    if (mInterface == NULL)
        mInterface = interface;

    GtkTreeView *lista;
    GtkListStore *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;

    lista = (GtkTreeView *) gtk_builder_get_object(interface, "tree_view_produtos");
    lista_estrutura = (GtkListStore *) gtk_tree_view_get_model(lista);

    gtk_list_store_clear(lista_estrutura);

    char preco[17];
    int i;
    for (i = 0; i < produtos->tamanho; i++){
        sprintf(preco, "R$ %.2lf", produtos->produtos[i].preco);
        
        gtk_list_store_append(lista_estrutura, &lista_estrutura_primaria);
        gtk_list_store_set(lista_estrutura, &lista_estrutura_primaria, 0, produtos->produtos[i].nome, 1, produtos->produtos[i].EmEstoque, 2, preco, -1);
    }

    produtos_botao(interface);
}

void recarregar_aba_produtos(GtkBuilder *dialog, GtkBuilder *interface){
    dialogOpened = false;
    if (interface)
        produtos_aba(interface, PRODUTOS_LISTA);
}

void mensagem_simples_produtos(GtkWindow *janela, gchar *titulo, gchar *mensagem){
    GtkWidget *dialog;
    GtkDialogFlags flags;

    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_message_dialog_new(janela, flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, mensagem);
    gtk_window_set_title((GtkWindow *) dialog, titulo);

    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_produtos), NULL);
    gtk_widget_show_all(dialog);
}

void novo_produto_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkBuilder *dialog_interface;
    const gchar dialog_interface_nome[] = "interfaces/novoproduto.xml";
    GtkWidget *window, *dialog, *submit;
    GError *erro = NULL;

    if (dialogOpened)
        return;
    else
        dialogOpened = true;
    
    dialog_interface = gtk_builder_new();
    if (!gtk_builder_add_from_file(dialog_interface, dialog_interface_nome, &erro)){
        fprintf(stderr, "Nao foi possivel inicializar a GUI: %s\n", erro->message);
        return;
    }

    window = (GtkWidget *) gtk_builder_get_object(interface, "window");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");
    submit = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputSubmit");

    if (!window || !dialog || !submit){
        fprintf(stderr, "Nao ha nenhum objeto com este id.\n");
        return;
    }
    
    gtk_window_set_title((GtkWindow *) dialog, "Novo Produto");

    gtk_window_set_transient_for((GtkWindow *) dialog, (GtkWindow *) window);
    gtk_widget_show(dialog);

    g_signal_connect(submit, "clicked", G_CALLBACK(novo_produto_callback), dialog_interface);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_produtos), interface);
}

void editar_produto_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkBuilder *dialog_interface;
    const gchar dialog_interface_nome[] = "interfaces/novoproduto.xml";
    GError *erro = NULL;

    GtkTreeView *lista;
    GtkTreeSelection *lista_seletor;
    GtkTreeModel *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;
    GtkWidget *window, *submit, *dialog, *nome, *em_estoque, *preco;

    struct produto **produto_busca;
    struct produto editar_produto;
    char *termo = (char *) malloc(100  * sizeof(char));
    int quantidadeProdutos;

    dialog_interface = gtk_builder_new();
    if (!gtk_builder_add_from_file(dialog_interface, dialog_interface_nome, &erro)){
        fprintf(stderr, "Nao foi possivel inicializar a GUI: %s\n", erro->message);
        return;
    }

    lista = (GtkTreeView *) gtk_builder_get_object(interface, "tree_view_produtos");
    lista_seletor = gtk_tree_view_get_selection(lista);
    lista_estrutura = gtk_tree_view_get_model(lista);
    window = (GtkWidget *) gtk_builder_get_object(interface, "window");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");

    gtk_window_set_title((GtkWindow *) dialog, "Editar Produto");

    if (!gtk_tree_selection_get_selected(lista_seletor, &lista_estrutura, &lista_estrutura_primaria)){
        mensagem_simples_produtos((GtkWindow *) dialog, "Atenção", "Selecione algum produto para editar.");
        return;
    } else
        if (dialogOpened)
            return;
        else
            dialogOpened = true;

    gtk_tree_model_get(lista_estrutura, &lista_estrutura_primaria, 0, &termo, -1);
    produto_busca = buscar_produto(PRODUTOS_LISTA, termo, &quantidadeProdutos);

    nome = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputNome");
    em_estoque = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputEmEstoque");
    preco = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputPreco");
    submit = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputSubmit");

    gtk_entry_set_text((GtkEntry *) nome, produto_busca[0]->nome);
    gtk_spin_button_set_value((GtkSpinButton *) em_estoque, produto_busca[0]->EmEstoque);
    gtk_spin_button_set_value((GtkSpinButton *) preco, produto_busca[0]->preco);

    gtk_window_set_transient_for((GtkWindow *) dialog, (GtkWindow *) window);
    gtk_widget_show(dialog);

    void **param = (void **) malloc(2 * sizeof(void *));
    param[0] = produto_busca[0];
    param[1] = dialog_interface;

    g_signal_connect(submit, "clicked", G_CALLBACK(editar_produto_callback), param);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_produtos), interface);
    free(termo);
}

void remover_produto_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkWidget *dialog, *window;
    GtkDialogFlags flags;
    GtkTreeView *lista;
    GtkTreeSelection *lista_seletor;
    GtkTreeModel *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;

    char *termo;
    struct produto **produto_busca;
    int quantidadeProdutos;

    window = (GtkWidget *) gtk_builder_get_object(interface, "window");
    lista = (GtkTreeView *) gtk_builder_get_object(interface, "tree_view_produtos");
    lista_seletor = gtk_tree_view_get_selection(lista);
    lista_estrutura = gtk_tree_view_get_model(lista);
    termo = (char *) malloc(100 * sizeof(char));

    if (!gtk_tree_selection_get_selected(lista_seletor, &lista_estrutura, &lista_estrutura_primaria)){
        mensagem_simples_produtos((GtkWindow *) window, "Atenção", "Selecione algum produto para remover.");
        return;
    } else
        if (dialogOpened)
            return;
        else
            dialogOpened = true;

    gtk_tree_model_get(lista_estrutura, &lista_estrutura_primaria, 0, &termo, -1);
    produto_busca = buscar_produto(PRODUTOS_LISTA, termo, &quantidadeProdutos);

    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_message_dialog_new((GtkWindow *) window, flags, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Você tem certeza em remover o produto %s?", produto_busca[0]->nome);
    gtk_dialog_add_button((GtkDialog *) dialog, "Sim, remover", 0);
    gtk_dialog_add_button((GtkDialog *) dialog, "Não, cancelar", 1);
    gtk_widget_show_all(dialog);

    g_signal_connect(dialog, "response", G_CALLBACK(remover_produto_callback), &(produto_busca[0]->index));
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_produtos), interface);
    free(termo);
}

void novo_produto_callback(GtkWidget *botao, GtkBuilder *dialog_interface){
    struct produto novo_produto;

    GtkWidget *nome, *em_estoque, *preco, *dialog;
    nome = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputNome");
    em_estoque = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputEmEstoque");
    preco = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputPreco");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");

    strcpy(novo_produto.nome, gtk_entry_get_text((GtkEntry *) nome));
    novo_produto.EmEstoque = (int) gtk_spin_button_get_value_as_int((GtkSpinButton *) em_estoque);
    novo_produto.preco = (double) gtk_spin_button_get_value((GtkSpinButton *) preco);

    if (strlen(novo_produto.nome) == 0)
        mensagem_simples_produtos((GtkWindow *) dialog, "Erro", "Todos os campos devem ser preenchidos.");
    else if (!adicionar_produto(PRODUTOS_LISTA, novo_produto))
        mensagem_simples_produtos((GtkWindow *) dialog, "Erro", "Já existe um produto com este nome.");
    else
        gtk_window_close((GtkWindow *) dialog);
}

void editar_produto_callback(GtkWidget *botao, void **param){
    struct produto *novo_produto = (struct produto *) param[0];
    GtkBuilder *dialog_interface = (GtkBuilder *) param[1];

    GtkWidget *nome, *em_estoque, *preco, *dialog;
    nome = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputNome");
    em_estoque = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputEmEstoque");
    preco = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputPreco");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");

    if (strlen(gtk_entry_get_text((GtkEntry *) nome)) == 0)
        mensagem_simples_produtos((GtkWindow *) dialog, "Erro", "Todos os campos devem ser preenchidos.");
    else {
        strcpy(novo_produto->nome, gtk_entry_get_text((GtkEntry *) nome));
        novo_produto->EmEstoque = gtk_spin_button_get_value_as_int((GtkSpinButton *) em_estoque);
        novo_produto->preco = gtk_spin_button_get_value((GtkSpinButton *) preco);

        modificar_produto(PRODUTOS_LISTA, novo_produto->index, novo_produto);
        gtk_window_close((GtkWindow *) dialog);
    }
}

void remover_produto_callback(GtkWidget *dialog, int id, int *index){
    if (id == 0)
        remover_produto(PRODUTOS_LISTA, *index);
    gtk_widget_destroy(dialog);
}

void produtos_botao(GtkBuilder *interface){
    GtkWidget *botao_novo_produto, *botao_editar_produto, *botao_remover_produto;
    
    botao_novo_produto = (GtkWidget *) gtk_builder_get_object(interface, "botao_novo_produto");
    gtk_tool_item_set_tooltip_text((GtkToolItem *) botao_novo_produto, "Adicionar Produto");
    g_signal_connect(botao_novo_produto, "clicked", G_CALLBACK(novo_produto_dialog), interface);

    botao_editar_produto = (GtkWidget *) gtk_builder_get_object(interface, "botao_editar_produto");
    gtk_tool_item_set_tooltip_text((GtkToolItem *) botao_editar_produto, "Editar Produto");
    g_signal_connect(botao_editar_produto, "clicked", G_CALLBACK(editar_produto_dialog), interface);

    botao_remover_produto = (GtkWidget *) gtk_builder_get_object(interface, "botao_remover_produto");
    gtk_tool_item_set_tooltip_text((GtkToolItem *) botao_remover_produto, "Remover Produto");
    g_signal_connect(botao_remover_produto, "clicked", G_CALLBACK(remover_produto_dialog), interface);
}