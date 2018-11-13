//  ;=====================================
//  ;  Title: vendas.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 11/11/18
//  ;=====================================

#include <gtk/gtk.h>
#include <stdbool.h>

#include "headers/vendas.h"
#include "headers/clientes.h"

static Vendas *VENDAS_LISTA = NULL;

void vendas_gui(GtkBuilder *, Vendas *);
void vendas_aba(GtkBuilder *, Vendas *);

void vendas_botao(GtkBuilder *);
void vendas_cliente_completion(GtkBuilder *);

void nova_venda_dialog(GtkWidget *, GtkBuilder *);

void vendas_gui(GtkBuilder *interface, Vendas *vendas){
    VENDAS_LISTA = vendas;
    vendas_aba(interface, vendas);
}

void vendas_aba(GtkBuilder *interface, Vendas * vendas){
    vendas_botao(interface);
}

void nova_venda_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkBuilder *dialog_interface;
    const gchar dialog_interface_nome[] = "interfaces/novavenda.xml";
    GtkWidget *window, *dialog;
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
    
    if (!window || !dialog){
        fprintf(stderr, "Nao ha nenhum objeto com este id.\n");
        return;
    }
    
    vendas_cliente_completion(dialog_interface);

    gtk_window_set_title((GtkWindow *) dialog, "Nova Venda");

    gtk_window_set_transient_for((GtkWindow *) dialog, (GtkWindow *) window);
    gtk_widget_show(dialog);
}

void vendas_botao(GtkBuilder *interface){
    GtkWidget *botao_nova_venda;

    botao_nova_venda = (GtkWidget *) gtk_builder_get_object(interface, "botao_nova_venda");
    gtk_tool_item_set_tooltip_text((GtkToolItem *) botao_nova_venda, "Nova Venda");
    g_signal_connect(botao_nova_venda, "clicked", G_CALLBACK(nova_venda_dialog), interface);
}

void vendas_cliente_completion(GtkBuilder *dialog_interface){
    GtkWidget *inputCliente;
    GtkTreeModel *lista;
    GtkTreeIter lista_estrutura_primaria;

    inputCliente = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputCliente");
    lista = (GtkTreeModel *) gtk_list_store_new(1, G_TYPE_STRING);

    int i;
    for (i = 0; i < CLIENTES_LISTA->tamanho; i++){
        gtk_list_store_append((GtkListStore *) lista, &lista_estrutura_primaria);
        gtk_list_store_set((GtkListStore *) lista, &lista_estrutura_primaria, 0, CLIENTES_LISTA->clientes[i].nome, -1);
    }

    GtkEntryCompletion *inputClienteCompletion = gtk_entry_completion_new();
    gtk_entry_completion_set_model(inputClienteCompletion, lista);
    gtk_entry_completion_set_text_column(inputClienteCompletion, 0);
    gtk_entry_completion_set_minimum_key_length(inputClienteCompletion, 1);
    gtk_entry_completion_set_popup_completion(inputClienteCompletion, TRUE);

    gtk_entry_set_completion((GtkEntry *) inputCliente, inputClienteCompletion);
}