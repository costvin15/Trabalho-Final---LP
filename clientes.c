//  ;=====================================
//  ;  Title: clientes.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 09/11/18
//  ;=====================================

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gtk/gtk.h>
#include "headers/clientes.h"

#define QUANTIDADE_ESTADOS_BRASILEIROS 27
#define QUANTIDADE_TIPOS_LOGRADOUROS 15

static Clientes *CLIENTES_LISTA = NULL;
static int dialogOpened = false;

void clientes_gui(GtkBuilder *, Clientes *);
void clientes_aba(GtkBuilder *, Clientes *);
void recarregar_aba_clientes(GtkDialog *, GtkBuilder *);
void mensagem_simples_clientes(GtkWindow *, gchar *, gchar *);

void clientes_botao(GtkBuilder *);

void novo_cliente_dialog(GtkWidget *, GtkBuilder *);
void novo_cliente_callback(GtkWidget *, GtkBuilder *);

void editar_cliente_dialog(GtkWidget *, GtkBuilder *);
void editar_cliente_callback(GtkWidget *, void**);

void remover_cliente_dialog(GtkWidget *, GtkBuilder *);
void remover_cliente_callback(GtkWidget *, int, int *);

void clientes_gui(GtkBuilder *interface, Clientes *clientes){
    CLIENTES_LISTA = clientes;
    clientes_aba(interface, clientes);
}

void clientes_aba(GtkBuilder *interface, Clientes *clientes){
    GtkTreeView *lista;
    GtkListStore *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;

    lista = (GtkTreeView *) gtk_builder_get_object(interface, "tree_view_clientes");
    lista_estrutura = (GtkListStore *) gtk_tree_view_get_model(lista);
    
    gtk_list_store_clear(lista_estrutura);

    char telefone[15];
    char endereco[210];

    int i;
    for (i = 0; i < clientes->tamanho; i++){
        sprintf(telefone, "(%u) %s", clientes->clientes[i].telefone.ddd, clientes->clientes[i].telefone.telefone);
        sprintf(endereco, "%s %s - %u, %s, %s - %s", clientes->clientes[i].endereco.logradouro, clientes->clientes[i].endereco.endereco, clientes->clientes[i].endereco.casa, clientes->clientes[i].endereco.bairro, clientes->clientes[i].endereco.cidade, clientes->clientes[i].endereco.estado);

        gtk_list_store_append(lista_estrutura, &lista_estrutura_primaria);
        gtk_list_store_set(lista_estrutura, &lista_estrutura_primaria, 0, clientes->clientes[i].nome, 1, telefone, 2, endereco, -1);
    }

    clientes_botao(interface);
}

void recarregar_aba_clientes(GtkDialog *dialog, GtkBuilder *interface){
    dialogOpened = false;
    if (interface)
        clientes_aba(interface, CLIENTES_LISTA);
}

void mensagem_simples_clientes(GtkWindow *janela, gchar *titulo, gchar *mensagem){
    GtkWidget *dialog;
    GtkDialogFlags flags;

    if (dialogOpened)
        return;
    else
        dialogOpened = true;

    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_message_dialog_new(janela, flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, mensagem);
    gtk_window_set_title((GtkWindow *) dialog, titulo);
    
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_clientes), NULL);
    gtk_widget_show_all(dialog);
}

void novo_cliente_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkBuilder *dialog_interface;
    const gchar dialog_interface_nome[] = "interfaces/novocliente.xml";
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
    
    gtk_window_set_title((GtkWindow *) dialog, "Novo Cliente");

    if (!window || !dialog || !submit){
        fprintf(stderr, "Nao ha nenhum objeto com este id.\n");
        return;
    }

    gtk_window_set_transient_for((GtkWindow *) dialog, (GtkWindow *) window);
    gtk_widget_show(dialog);

    g_signal_connect(submit, "clicked", G_CALLBACK(novo_cliente_callback), dialog_interface);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_clientes), interface);
}

void editar_cliente_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkBuilder *dialog_interface;
    const gchar dialog_interface_nome[] = "interfaces/novocliente.xml";
    GError *erro = NULL;
    
    GtkTreeView *lista;
    GtkTreeSelection *lista_seletor;
    GtkTreeModel *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;
    GtkWidget *window, *submit, *dialog, *nome, *ddd, *telefone, *logradouro, *endereco, *numero, *bairro, *cidade, *estado;

    struct cliente **cliente_busca;
    struct cliente editar_cliente;
    char *termo = (char *) malloc(100 * sizeof(char));
    int quantidadeClientes;
    const char *estados[] = {"Acre", "Alagoas", "Amapá", "Amazonas", "Bahia", "Ceará", "Distrito Federal", "Espírito Santo", "Goiás", "Maranhão", "Mato Grosso", "Mato Grosso do Sul", "Minas Gerais", "Pará", "Paraíba", "Paraná", "Pernambuco", "Piauí", "Rio de Janeiro", "Rio Grande do Norte", "Rio Grande do Sul", "Rondônia", "Roraima", "Santa Catarina", "São Paulo", "Sergipe", "Tocantins"};
    const char *logradouros[] = {"Rua", "Alameda", "Avenida", "Chácara", "Colônia", "Conjunto", "Distrito", "Esplanada", "Estrada", "Largo", "Loteamento", "Sítio", "Travessa", "Via", "Vila"};

    dialog_interface = gtk_builder_new();
    if (!gtk_builder_add_from_file(dialog_interface, dialog_interface_nome, &erro)){
        fprintf(stderr, "Nao foi possivel inicializar a GUI: %s\n", erro->message);
        return;
    }

    lista = (GtkTreeView *) gtk_builder_get_object(interface, "tree_view_clientes");
    lista_seletor = gtk_tree_view_get_selection(lista);
    lista_estrutura = gtk_tree_view_get_model(lista);
    window = (GtkWidget *) gtk_builder_get_object(interface, "window");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");

    gtk_window_set_title((GtkWindow *) dialog, "Editar Cliente");

    if (!gtk_tree_selection_get_selected(lista_seletor, &lista_estrutura, &lista_estrutura_primaria)){
        mensagem_simples_clientes((GtkWindow *) dialog, "Atenção", "Selecione algum cliente para editar.");
        return;
    } else
        if (dialogOpened)
            return;
        else
            dialogOpened = true;
    
    gtk_tree_model_get(lista_estrutura, &lista_estrutura_primaria, 0, &termo, -1);
    cliente_busca = buscar_cliente(CLIENTES_LISTA, termo, &quantidadeClientes);

    nome = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputNome");
    ddd = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputDDD");
    telefone = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputTelefone");
    logradouro = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputLogradouro");
    endereco = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputEndereco");
    numero = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputNumero");
    bairro = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputBairro");
    cidade = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputCidade");
    estado = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputEstado");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");
    submit = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputSubmit");

    gtk_entry_set_text((GtkEntry *) nome, cliente_busca[0]->nome);
    gtk_spin_button_set_value((GtkSpinButton *) ddd, cliente_busca[0]->telefone.ddd);
    gtk_entry_set_text((GtkEntry *) telefone, cliente_busca[0]->telefone.telefone);
    gtk_entry_set_text((GtkEntry *) endereco, cliente_busca[0]->endereco.endereco);
    gtk_spin_button_set_value((GtkSpinButton *) numero, cliente_busca[0]->endereco.casa);
    gtk_entry_set_text((GtkEntry *) bairro, cliente_busca[0]->endereco.bairro);
    gtk_entry_set_text((GtkEntry *) cidade, cliente_busca[0]->endereco.cidade);

    int i;
    for (i = 0; i < QUANTIDADE_ESTADOS_BRASILEIROS; i++)
        if (strcmp(cliente_busca[0]->endereco.estado, estados[i]) == 0){
            gtk_combo_box_set_active((GtkComboBox *) estado, i);
            break;
        }
    
    for (i = 0; i < QUANTIDADE_TIPOS_LOGRADOUROS; i++)
        if (strcmp(cliente_busca[0]->endereco.logradouro, logradouros[i]) == 0){
            gtk_combo_box_set_active((GtkComboBox *) logradouro, i);
            break;
        }

    gtk_window_set_transient_for((GtkWindow *) dialog, (GtkWindow *) window);
    gtk_widget_show(dialog);

    void **param = (void **) malloc(2 * sizeof(void *));
    param[0] = cliente_busca[0];
    param[1] = dialog_interface;

    g_signal_connect(submit, "clicked", G_CALLBACK(editar_cliente_callback), param);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_clientes), interface);
    free(termo);
}

void remover_cliente_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkWidget *dialog, *window;
    GtkDialogFlags flags;
    GtkTreeView *lista;
    GtkTreeSelection *lista_seletor;
    GtkTreeModel *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;
    
    char *termo;
    struct cliente **cliente_busca;
    int quantidadeClientes;
    
    window = (GtkWidget *) gtk_builder_get_object(interface, "window");
    lista = (GtkTreeView *) gtk_builder_get_object(interface, "tree_view_clientes");
    lista_seletor = gtk_tree_view_get_selection(lista);
    lista_estrutura = gtk_tree_view_get_model(lista);
    termo  = (char *) malloc(100 * sizeof(char));

    if (!gtk_tree_selection_get_selected(lista_seletor, &lista_estrutura, &lista_estrutura_primaria)){
        mensagem_simples_clientes((GtkWindow *) window, "Atenção", "Selecione algum cliente para remover.");
        return;
    } else
        if (dialogOpened)
            return;
        else
            dialogOpened = true;

    gtk_tree_model_get(lista_estrutura, &lista_estrutura_primaria, 0, &termo, -1);
    cliente_busca = buscar_cliente(CLIENTES_LISTA, termo, &quantidadeClientes);

    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_message_dialog_new((GtkWindow *) window, flags, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Você tem certeza em remover o cliente %s?", cliente_busca[0]->nome);
    gtk_dialog_add_button((GtkDialog *) dialog, "Sim, remover", 0);
    gtk_dialog_add_button((GtkDialog *) dialog, "Não, cancelar", 1);
    gtk_widget_show_all(dialog);

    g_signal_connect(dialog, "response", G_CALLBACK(remover_cliente_callback), &(cliente_busca[0]->index));
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_clientes), interface);
}

void novo_cliente_callback(GtkWidget *botao, GtkBuilder *dialog_interface){
    struct cliente novo_cliente;

    GtkWidget *nome, *ddd, *telefone, *logradouro, *endereco, *numero, *bairro, *cidade, *estado, *dialog;
    nome = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputNome");
    ddd = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputDDD");
    telefone = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputTelefone");
    logradouro = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputLogradouro");
    endereco = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputEndereco");
    numero = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputNumero");
    bairro = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputBairro");
    cidade = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputCidade");
    estado = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputEstado");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");

    strcpy(novo_cliente.nome, gtk_entry_get_text((GtkEntry *) nome));
    novo_cliente.telefone.ddd = (unsigned short) gtk_spin_button_get_value_as_int((GtkSpinButton *) ddd);
    strcpy(novo_cliente.telefone.telefone, gtk_entry_get_text((GtkEntry *) telefone));
    strcpy(novo_cliente.endereco.logradouro, gtk_combo_box_text_get_active_text((GtkComboBoxText *) logradouro));
    strcpy(novo_cliente.endereco.endereco, gtk_entry_get_text((GtkEntry *) endereco));
    novo_cliente.endereco.casa = (unsigned short) gtk_spin_button_get_value_as_int((GtkSpinButton *) numero);
    strcpy(novo_cliente.endereco.bairro, gtk_entry_get_text((GtkEntry *) bairro));
    strcpy(novo_cliente.endereco.cidade, gtk_entry_get_text((GtkEntry *) cidade));
    strcpy(novo_cliente.endereco.estado, gtk_combo_box_text_get_active_text((GtkComboBoxText *) estado));

    if (strlen(novo_cliente.nome) == 0 ||
        strlen(novo_cliente.telefone.telefone) == 0 ||
        strlen(novo_cliente.endereco.endereco) == 0 ||
        strlen(novo_cliente.endereco.bairro) == 0 ||
        strlen(novo_cliente.endereco.cidade) == 0)
        mensagem_simples_clientes((GtkWindow *) dialog, "Erro", "Todos os campos devem ser preenchidos.");
    else if(!adicionar_cliente(CLIENTES_LISTA, novo_cliente))
        mensagem_simples_clientes((GtkWindow *) dialog, "Erro", "Já existe um cliente com este nome.");
    else
        gtk_window_close((GtkWindow *) dialog);
}

void editar_cliente_callback(GtkWidget *botao, void **param){
    struct cliente *novo_cliente = (struct cliente *) param[0];
    GtkBuilder *dialog_interface = (GtkBuilder *) param[1];

    GtkWidget *nome, *ddd, *telefone, *logradouro, *endereco, *numero, *bairro, *cidade, *estado, *dialog;
    nome = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputNome");
    ddd = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputDDD");
    telefone = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputTelefone");
    logradouro = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputLogradouro");
    endereco = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputEndereco");
    numero = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputNumero");
    bairro = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputBairro");
    cidade = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputCidade");
    estado = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputEstado");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");

    if (strlen(gtk_entry_get_text((GtkEntry *) nome)) == 0 ||
        strlen(gtk_entry_get_text((GtkEntry *) telefone)) == 0 ||
        strlen(gtk_entry_get_text((GtkEntry *) endereco)) == 0 ||
        strlen(gtk_entry_get_text((GtkEntry *) bairro)) == 0 ||
        strlen(gtk_entry_get_text((GtkEntry *) cidade)) == 0){
        mensagem_simples_clientes((GtkWindow *) dialog, "Erro", "Todos os campos devem ser preenchidos.");
    } else {
        strcpy(novo_cliente->nome, gtk_entry_get_text((GtkEntry *) nome));
        novo_cliente->telefone.ddd = (unsigned short) gtk_spin_button_get_value((GtkSpinButton *) ddd);
        strcpy(novo_cliente->telefone.telefone, gtk_entry_get_text((GtkEntry *) telefone));
        strcpy(novo_cliente->endereco.logradouro, gtk_combo_box_text_get_active_text((GtkComboBoxText *) logradouro));
        strcpy(novo_cliente->endereco.endereco, gtk_entry_get_text((GtkEntry *) endereco));
        novo_cliente->endereco.casa = (unsigned short) gtk_spin_button_get_value((GtkSpinButton *) numero);
        strcpy(novo_cliente->endereco.bairro, gtk_entry_get_text((GtkEntry *) bairro));
        strcpy(novo_cliente->endereco.cidade, gtk_entry_get_text((GtkEntry *) cidade));
        strcpy(novo_cliente->endereco.estado, gtk_combo_box_text_get_active_text((GtkComboBoxText *) estado));

        modificar_cliente(CLIENTES_LISTA, novo_cliente->index, novo_cliente);
        gtk_window_close((GtkWindow *) dialog);
    }
}

void remover_cliente_callback(GtkWidget *dialog, int id, int *index){
    if (id == 0)
        remover_cliente(CLIENTES_LISTA, *index);
    gtk_widget_destroy(dialog);
}

void clientes_botao(GtkBuilder *interface){
    GtkWidget *botao_novo_cliente, *botao_editar_cliente, *botao_remover_cliente;

    botao_novo_cliente = (GtkWidget *) gtk_builder_get_object(interface, "botao_novo_cliente");
    g_signal_connect(botao_novo_cliente, "clicked", G_CALLBACK(novo_cliente_dialog), interface);

    botao_editar_cliente = (GtkWidget *) gtk_builder_get_object(interface, "botao_editar_cliente");
    g_signal_connect(botao_editar_cliente, "clicked", G_CALLBACK(editar_cliente_dialog), interface);

    botao_remover_cliente = (GtkWidget *) gtk_builder_get_object(interface, "botao_remover_cliente");
    g_signal_connect(botao_remover_cliente, "clicked", G_CALLBACK(remover_cliente_dialog), interface);
}