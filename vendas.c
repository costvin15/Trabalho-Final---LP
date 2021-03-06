//  ;=====================================
//  ;  Title: vendas.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 11/11/18
//  ;=====================================

#include <gtk/gtk.h>

#include <time.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>

#include "pdfgen/pdfgen.h"

#include "headers/vendas.h"
#include "headers/clientes.h"
#include "headers/produtos.h"

static Vendas *VENDAS_LISTA = NULL;
static Carrinho *CARRINHO = NULL;

void vendas_gui(GtkBuilder *, Vendas *);
void vendas_aba(GtkBuilder *, Vendas *);
void recarregar_aba_vendas(GtkBuilder *, GtkBuilder *);

void vendas_botao(GtkBuilder *);
void vendas_cliente_completion(GtkWidget *);
void vendas_produto_completion(GtkWidget *);
void mensagem_simples_vendas(GtkWindow *, gchar *, gchar *);
void vendas_clique_duplo(GtkTreeView *, GtkTreePath *, GtkTreeViewColumn *, GtkBuilder *);

void atualizar_valor_final(GtkWidget *, GtkBuilder *);
void nova_venda_dialog(GtkWidget *, GtkBuilder *);
void nova_venda_callback(GtkWidget *, GtkBuilder *);

void carrinho_dialog(GtkWidget *, GtkBuilder *);
void recarregar_lista_carrinho(GtkWidget *, int, GtkTreeView *);
void carrinho_callback(GtkWidget *, GtkBuilder *);
void carrinho_remover_clique_duplo(GtkTreeView *, GtkTreePath *, GtkTreeViewColumn *, GtkBuilder *);
void carrinho_remover_callback(GtkWidget *, int, gpointer);

void remover_venda_dialog(GtkWidget *, GtkBuilder *);
void remover_venda_callback(GtkWidget *, int, int *);

void relatorio_dialog(GtkWidget *, GtkBuilder *);
void relatorio_callback(GtkWidget *, GtkBuilder *);

void vendas_gui(GtkBuilder *interface, Vendas *vendas){
    VENDAS_LISTA = vendas;
    CARRINHO = criar_carrinho();
    vendas_aba(interface, vendas);
}

char *asctime(const struct tm *timeptr){
    static const char wday_name[][4] = { "Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab" };
    static const char mon_name[][4] = { "Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez" };
    static char result[27];
    sprintf(result, "%.3s,%3d %.3s %d %.2d:%.2d:%.2d",
        wday_name[timeptr->tm_wday],
        timeptr->tm_mday,
        mon_name[timeptr->tm_mon],
        1900 + timeptr->tm_year,
        timeptr->tm_hour,
        timeptr->tm_min,
        timeptr->tm_sec
    );
    return result;
}

void vendas_aba(GtkBuilder *interface, Vendas * vendas){
    GtkTreeView *lista;
    GtkListStore *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;

    lista = (GtkTreeView *) gtk_builder_get_object(interface, "tree_view_vendas");
    lista_estrutura = (GtkListStore *) gtk_tree_view_get_model(lista);

    gtk_list_store_clear(lista_estrutura);

    char produtos[203];
    char preco[17];
    char *horario;
    struct tm *horario_info;
    int i;

    for (i = 0; i < vendas->tamanho; i++){
        horario_info = localtime(&vendas->vendas[i].horario);
        horario = asctime(horario_info);

        if (vendas->vendas[i].carrinho.tamanho > 2)
            sprintf(produtos, "%s, %s...", vendas->vendas[i].carrinho.produtos[0].nome, vendas->vendas[i].carrinho.produtos[1].nome);
        else if (vendas->vendas[i].carrinho.tamanho > 1)
            sprintf(produtos, "%s, %s", vendas->vendas[i].carrinho.produtos[0].nome, vendas->vendas[i].carrinho.produtos[1].nome);
        else
            sprintf(produtos, "%s", vendas->vendas[i].carrinho.produtos[0].nome);
        sprintf(preco, "R$ %.2lf", vendas->vendas[i].carrinho.preco_final);

        gtk_list_store_append(lista_estrutura, &lista_estrutura_primaria);
        gtk_list_store_set(lista_estrutura, &lista_estrutura_primaria, 0, vendas->vendas[i].cliente.nome, 1, produtos, 2, preco, 3, horario, -1);
    }

    g_signal_connect(lista, "row-activated", G_CALLBACK(vendas_clique_duplo), interface);
    vendas_botao(interface);
}

void recarregar_aba_vendas(GtkBuilder *dialog, GtkBuilder *interface){
    dialogOpened = false;
    if (interface)
        vendas_aba(interface, VENDAS_LISTA);
}

void nova_venda_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkBuilder *dialog_interface;
    const gchar dialog_interface_nome[] = "interfaces/novavenda.xml";
    GtkWidget *window, *dialog, *inputCliente, *inputSubmit, *inputPrecoFinal, *inputDesconto;
    GtkTreeView *lista;
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
    inputCliente = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputCliente");
    lista = (GtkTreeView *) gtk_builder_get_object(dialog_interface, "tree_view_carrinho");
    inputSubmit = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputSubmit");
    inputPrecoFinal = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputPrecoFinal");
    inputDesconto = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputDesconto");

    if (!window || !dialog || !inputCliente || !lista || !inputCliente){
        fprintf(stderr, "Nao ha nenhum objeto com este id.\n");
        return;
    }
    
    gtk_spin_button_set_value((GtkSpinButton *) inputDesconto, CARRINHO->descontopercento);
    
    char preco[60];
    sprintf(preco, "Preço Final: R$ %.2lf - Desconto: R$ %.2f", CARRINHO->preco_final, CARRINHO->desconto);
    gtk_label_set_text((GtkLabel *) inputPrecoFinal, preco);

    recarregar_lista_carrinho(NULL, 0, lista);

    vendas_cliente_completion(inputCliente);
    gtk_window_set_title((GtkWindow *) dialog, "Nova Venda");

    gtk_window_set_transient_for((GtkWindow *) dialog, (GtkWindow *) window);
    gtk_widget_show(dialog);

    g_signal_connect(inputDesconto, "value-changed", G_CALLBACK(atualizar_valor_final), dialog_interface);
    g_signal_connect(inputSubmit, "clicked", G_CALLBACK(nova_venda_callback), dialog_interface);
    g_signal_connect(inputSubmit, "clicked", G_CALLBACK(recarregar_aba_vendas), interface);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_vendas), interface);
}

void atualizar_valor_final(GtkWidget *inputDesconto, GtkBuilder *dialog_interface){
    GtkWidget *inputPrecoFinal;
    inputDesconto = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputDesconto");
    inputPrecoFinal = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputPrecoFinal");
    char preco[60];

    definir_desconto(CARRINHO, gtk_spin_button_get_value((GtkSpinButton *) inputDesconto));

    sprintf(preco, "Preço Final: R$ %.2lf - Desconto: R$ %.2f", CARRINHO->preco_final, CARRINHO->desconto);
    gtk_label_set_text((GtkLabel *) inputPrecoFinal, preco);
}

void nova_venda_callback(GtkWidget *botao, GtkBuilder *dialog_interface){
    GtkWidget *dialog, *inputCliente, *inputDesconto, *inputPrecoFinal;
    GtkTreeView *lista;

    lista = (GtkTreeView *) gtk_builder_get_object(dialog_interface, "tree_view_carrinho");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");
    inputCliente = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputCliente");
    inputDesconto = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputDesconto");

    if (!strlen((char *) gtk_entry_get_text((GtkEntry *) inputCliente)) > 0){
        mensagem_simples_vendas((GtkWindow *) dialog, "Atenção", "Digite o nome de algum cliente.");
        return;
    }

    int i, ocorrencias;
    struct cliente **clientes = buscar_cliente(CLIENTES_LISTA, (char *) gtk_entry_get_text((GtkEntry *) inputCliente), &ocorrencias);
    struct produto **buscaProduto;

    if (ocorrencias > 0)
        if (!CARRINHO->tamanho > 0)
            mensagem_simples_vendas((GtkWindow *) dialog, "Erro", "O carrinho está vazio.");
        else if (strcmp((char *) gtk_entry_get_text((GtkEntry *) inputCliente), clientes[0]->nome) == 0){
            for (i = 0; i < CARRINHO->tamanho; i++){
                buscaProduto = buscar_produto(PRODUTOS_LISTA, CARRINHO->produtos[i].nome, &ocorrencias);
                if (ocorrencias <= 0){
                    mensagem_simples_vendas((GtkWindow *) dialog, "Erro", "Erro desconhecido.");
                    return;
                }
                
                if (buscaProduto[0]->EmEstoque - CARRINHO->produtos[i].quantidade < 0){
                    mensagem_simples_vendas((GtkWindow *) dialog, "Atenção", "Você não tem produtos em estoque suficiente para vender.");
                    return;
                }

                buscaProduto[0]->EmEstoque -= CARRINHO->produtos[i].quantidade;
            }
            
            produtos_aba(NULL, NULL);

            nova_venda(VENDAS_LISTA, *clientes[0], CARRINHO);
            CARRINHO = criar_carrinho();
            gtk_widget_destroy(dialog);
        } else
            mensagem_simples_vendas((GtkWindow *) dialog, "Erro", "Não foi possível encontrar este cliente.");
    else
        mensagem_simples_vendas((GtkWindow *) dialog, "Erro", "Não foi possível encontrar este cliente.");
}

void mensagem_simples_vendas(GtkWindow *janela, gchar *titulo, gchar *mensagem){
    GtkWidget *dialog;
    GtkDialogFlags flags;

    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_message_dialog_new(janela, flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, mensagem);
    gtk_window_set_title((GtkWindow *) dialog, titulo);

    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_produtos), NULL);
    gtk_widget_show_all(dialog);
}

void vendas_clique_duplo(GtkTreeView *_lista, GtkTreePath *caminho, GtkTreeViewColumn *_coluna, GtkBuilder *interface){
    int *indices_caminho_lista;
    struct venda venda_atual;
    GtkBuilder *dialog_interface;
    const gchar dialog_interface_nome[] = "interfaces/novavenda.xml";
    GtkWidget *window, *dialog, *inputCliente, *inputDesconto, *inputPrecoFinal, *submit;
    GtkTreeView *lista;
    GtkListStore *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;
    GError *erro = NULL;

    if (dialogOpened)
        return;
    else
        dialogOpened = true;
    
    indices_caminho_lista = gtk_tree_path_get_indices(caminho);
    venda_atual = VENDAS_LISTA->vendas[indices_caminho_lista[0]];

    dialog_interface = gtk_builder_new();
    if (!gtk_builder_add_from_file(dialog_interface, dialog_interface_nome, &erro)){
        fprintf(stderr, "Nao foi possivel inicializar a GUI: %s\n", erro->message);
        return ;
    }

    window = (GtkWidget *) gtk_builder_get_object(interface, "window");
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");
    inputCliente = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputCliente");
    inputDesconto = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputDesconto");
    inputPrecoFinal = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputPrecoFinal");
    submit = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputSubmit");
    lista = (GtkTreeView *) gtk_builder_get_object(dialog_interface, "tree_view_carrinho");
    
    if (!window || !dialog || !inputCliente || !inputDesconto || !submit || !lista){
        fprintf(stderr, "Nao ha nenhum objeto com este id.\n");
        return;
    }

    g_object_set(inputCliente, "editable", FALSE, NULL);
    gtk_entry_set_text((GtkEntry *) inputCliente, venda_atual.cliente.nome);

    lista_estrutura = (GtkListStore *) gtk_tree_view_get_model(lista);
    gtk_list_store_clear(lista_estrutura);

    char preco[60];
    int i;
    for (i = 0; i < venda_atual.carrinho.tamanho; i++){
        sprintf(preco, "R$ %.2lf", venda_atual.carrinho.produtos[i].preco);

        gtk_list_store_append(lista_estrutura, &lista_estrutura_primaria);
        gtk_list_store_set(lista_estrutura, &lista_estrutura_primaria, 0, venda_atual.carrinho.produtos[i].nome, 1, venda_atual.carrinho.produtos[i].quantidade, 2, preco, -1);
    }
    
    gtk_widget_destroy(submit);

    g_object_set(inputDesconto, "editable", FALSE, NULL);
    gtk_spin_button_set_value((GtkSpinButton *) inputDesconto, venda_atual.carrinho.descontopercento);
    
    sprintf(preco, "Preço Final: R$ %.2lf - Desconto: R$ %.2f", venda_atual.carrinho.preco_final, venda_atual.carrinho.desconto);
    gtk_label_set_text((GtkLabel *) inputPrecoFinal, preco);

    gtk_window_set_title((GtkWindow *) dialog, venda_atual.cliente.nome);
    gtk_window_set_transient_for((GtkWindow *) dialog, (GtkWindow *) window);
    gtk_widget_show(dialog);
    
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_vendas), interface);
}

void carrinho_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkBuilder *dialog_interface;
    const gchar dialog_interface_nome[] = "interfaces/carrinho.xml";
    GtkWidget *window, *dialog, *inputProduto, *submit;
    GtkTreeView *lista;

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
    inputProduto = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputProduto");
    submit = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputSubmit");
    lista = (GtkTreeView *) gtk_builder_get_object(dialog_interface, "tree_view_carrinho");
    
    if (!window || !dialog || !inputProduto || !submit || !lista){
        fprintf(stderr, "Nao ha nenhum objeto com este id.\n");
        return;
    }

    recarregar_lista_carrinho(NULL, 0, lista);

    g_signal_connect(submit, "clicked", G_CALLBACK(carrinho_callback), dialog_interface);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_vendas), interface);
    g_signal_connect(lista, "row-activated", G_CALLBACK(carrinho_remover_clique_duplo), dialog_interface);

    vendas_produto_completion(inputProduto);

    gtk_window_set_title((GtkWindow *) dialog, "Carrinho");
    gtk_window_set_transient_for((GtkWindow *) dialog, (GtkWindow *) window);
    gtk_widget_show(dialog);
}

void carrinho_remover_clique_duplo(GtkTreeView *lista, GtkTreePath *caminho, GtkTreeViewColumn *coluna, GtkBuilder *dialog_interface){
    GtkWidget *dialog, *window;
    GtkDialogFlags flags;
    int *indices_caminho_lista;

    indices_caminho_lista = gtk_tree_path_get_indices(caminho);

    flags = GTK_DIALOG_DESTROY_WITH_PARENT;

    window = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");
    dialog = gtk_message_dialog_new((GtkWindow *) window, flags, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Você tem certeza em remover este item do carrinho?");

    gtk_dialog_add_button((GtkDialog *) dialog, "Sim, remover", 0);
    gtk_dialog_add_button((GtkDialog *) dialog, "Não, cancelar", 1);
    
    gtk_widget_show_all(dialog);

    g_signal_connect(dialog, "response", G_CALLBACK(carrinho_remover_callback), (gpointer) GINT_TO_POINTER(indices_caminho_lista[0]));
    g_signal_connect(dialog, "response", G_CALLBACK(recarregar_lista_carrinho), lista);
}

void carrinho_remover_callback(GtkWidget *dialog, int id, gpointer data){
    if (id != 0){    
        gtk_widget_destroy(dialog);
        return;
    }

    int indice = GPOINTER_TO_INT(data);
    remover_produto_carrinho(CARRINHO, indice);
}

void recarregar_lista_carrinho(GtkWidget *caller, int caller_id, GtkTreeView *lista){
    GtkListStore *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;

    lista_estrutura = (GtkListStore *) gtk_tree_view_get_model(lista);
    gtk_list_store_clear(lista_estrutura);

    char preco[17];
    int i;
    for (i = 0; i < CARRINHO->tamanho; i++){
        sprintf(preco, "R$ %.2lf", CARRINHO->produtos[i].preco);

        gtk_list_store_append(lista_estrutura, &lista_estrutura_primaria);
        gtk_list_store_set(lista_estrutura, &lista_estrutura_primaria, 0, CARRINHO->produtos[i].nome, 1, CARRINHO->produtos[i].quantidade, 2, preco, -1);
    }

    if (caller)
        gtk_widget_destroy(caller);
}

void carrinho_callback(GtkWidget *botao, GtkBuilder *dialog_interface){
    GtkWidget *dialog, *inputProduto, *inputQuantidade;
    GtkTreeView *lista;
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");
    inputProduto = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputProduto");
    inputQuantidade = (GtkWidget *) gtk_builder_get_object(dialog_interface, "inputQuantidade");
    lista = (GtkTreeView *) gtk_builder_get_object(dialog_interface, "tree_view_carrinho");

    if (!dialog || !inputProduto || !inputQuantidade){
        fprintf(stderr, "Nao ha nenhum objeto com este id.\n");
        return;
    }

    int ocorrencias, quantidade = gtk_spin_button_get_value((GtkSpinButton *) inputQuantidade);
    struct produto **produtos = buscar_produto(PRODUTOS_LISTA, (char *) gtk_entry_get_text((GtkEntry *) inputProduto), &ocorrencias);
    
    if (ocorrencias > 0)
        if (strcmp((char *) gtk_entry_get_text((GtkEntry *) inputProduto), produtos[0]->nome) == 0){
            if (produtos[0]->EmEstoque - quantidade >= 0){
                adicionar_produto_carrinho(CARRINHO, *produtos[0], quantidade);
                recarregar_lista_carrinho(NULL, 0, lista);
            } else
                mensagem_simples_vendas((GtkWindow *) dialog, "Atenção", "Você tem quantidade insuficiente no estoque.");
        } else
            mensagem_simples_vendas((GtkWindow *) dialog, "Erro", "Nao foi possivel encontrar este produto.");
    else
        mensagem_simples_vendas((GtkWindow *) dialog, "Erro", "Nao foi possivel encontrar este produto.");
}

void remover_venda_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkWidget *dialog, *window;
    GtkDialogFlags flags;
    GtkTreeView *lista;
    GtkTreePath *caminho;
    GtkTreeSelection *lista_seletor;
    GtkTreeModel *lista_estrutura;
    GtkTreeIter lista_estrutura_primaria;
    int *indices;

    window = (GtkWidget *) gtk_builder_get_object(interface, "window");
    lista = (GtkTreeView *) gtk_builder_get_object(interface, "tree_view_vendas");
    lista_seletor = gtk_tree_view_get_selection(lista);
    lista_estrutura = gtk_tree_view_get_model(lista);

    if (!gtk_tree_selection_get_selected(lista_seletor, &lista_estrutura, &lista_estrutura_primaria)){
        mensagem_simples_vendas((GtkWindow *) window, "Atenção", "Selecione alguma venda para remover.");
        return;
    } else
        if (dialogOpened)
            return;
        else
            dialogOpened = true;
    
    caminho = gtk_tree_model_get_path(lista_estrutura, &lista_estrutura_primaria);
    indices = gtk_tree_path_get_indices(caminho);

    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_message_dialog_new((GtkWindow *) window, flags, GTK_MESSAGE_QUESTION, GTK_BUTTONS_NONE, "Você tem certeza em remover a venda nº %d?", indices[0] + 1);
    gtk_dialog_add_button((GtkDialog *) dialog, "Sim, remover", 0);
    gtk_dialog_add_button((GtkDialog *) dialog, "Não, cancelar", 1);
    gtk_widget_show_all(dialog);

    g_signal_connect(dialog, "response", G_CALLBACK(remover_venda_callback), indices);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_vendas), interface);
}

void remover_venda_callback(GtkWidget *dialog, int id, int *indices){
    if (id == 0)
        remover_venda(VENDAS_LISTA, indices[0]);
    gtk_widget_destroy(dialog);
}

void relatorio_dialog(GtkWidget *botao, GtkBuilder *interface){
    GtkBuilder *dialog_interface;
    const gchar dialog_interface_nome[] = "interfaces/gerarrelatorio.xml";
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
    submit = (GtkWidget *) gtk_builder_get_object(dialog_interface, "submit");

    gtk_window_set_title((GtkWindow *) dialog, "Gerar Relatório");
    gtk_window_set_transient_for((GtkWindow *) dialog, (GtkWindow *) window);
    gtk_widget_show(dialog);

    g_signal_connect(submit, "clicked", G_CALLBACK(relatorio_callback), dialog_interface);
    g_signal_connect(dialog, "destroy", G_CALLBACK(recarregar_aba_vendas), interface);
}

int busca_binaria_relatorio(Vendas *lista, time_t termo, int inicio, int fim){
    int meio = (inicio + fim) / 2;

    if (lista->vendas[meio].horario == termo)
        return meio;
    if (inicio > fim)
        return inicio;
    else if (lista->vendas[meio].horario > termo)
        return busca_binaria_relatorio(lista, termo, inicio, meio - 1);
    else
        return busca_binaria_relatorio(lista, termo, meio + 1, fim);
}

void relatorio_callback(GtkWidget *botao, GtkBuilder *dialog_interface){
#ifndef _WIN32
    mensagem_simples_vendas((GtkWindow *) NULL, "Desculpe-nos.", "Infelizmente, esta funcionalidade esta disponivel por enquanto apenas para computadores Windows.");
    return;
#endif

    struct venda *relatorio;
    GtkWidget *dialog;
    GtkCalendar *calendario;
    struct tm data_estrutura = {0};
    time_t data;
    struct pdf_doc *relatorio_pdf_doc;

    setlocale(LC_ALL, "");

    relatorio = NULL;
    dialog = (GtkWidget *) gtk_builder_get_object(dialog_interface, "dialog");
    calendario = (GtkCalendar *) gtk_builder_get_object(dialog_interface, "calendario");
    gtk_calendar_get_date(calendario, &data_estrutura.tm_year, &data_estrutura.tm_mon, &data_estrutura.tm_mday);
    data_estrutura.tm_year -= 1900;

    data = mktime(&data_estrutura);

    int prox = busca_binaria_relatorio(VENDAS_LISTA, data, 0, VENDAS_LISTA->tamanho - 1);
    int encontrados = 0;

    while (VENDAS_LISTA->vendas[prox].horario >= data && VENDAS_LISTA->vendas[prox].horario <= data + 86400){
        relatorio = (struct venda *) realloc(relatorio, (encontrados + 1) * sizeof(struct venda));
        relatorio[encontrados] = VENDAS_LISTA->vendas[prox];

        encontrados++;
        prox++;
    }
    
    char nome_documento[250];
    strftime(nome_documento, sizeof(nome_documento), "Relatorio - Dia %d/%m/%Y", &data_estrutura);

    struct pdf_info relatorio_pdf_info = {
        .creator = "Loja NerdZ",
        .producer = "Loja NerdZ",
        .author = "Loja NerdZ",
    };

    strcpy(relatorio_pdf_info.title, nome_documento);
    strcpy(relatorio_pdf_info.subject, nome_documento);
    
    strftime(nome_documento, sizeof(nome_documento), "Relatório - Dia %d/%m/%Y", &data_estrutura);

    relatorio_pdf_doc = pdf_create(PDF_A4_WIDTH, PDF_A4_HEIGHT, &relatorio_pdf_info);
    struct pdf_object *pagina = pdf_append_page(relatorio_pdf_doc);
    pdf_set_font(relatorio_pdf_doc, "Helvetica");

    int alturaAtual = 770;
    char quantidade_vendas_documento[25];
    sprintf(quantidade_vendas_documento, "Foram realizadas %d venda(s) neste dia.", encontrados);
    
    pdf_set_font(relatorio_pdf_doc, "Helvetica-Bold");
    pdf_add_text_wrap(relatorio_pdf_doc, pagina, "Loja NerdZ", 20, 60, alturaAtual, PDF_RGB(0, 0, 0), 300, PDF_ALIGN_JUSTIFY);
    pdf_set_font(relatorio_pdf_doc, "Helvetica");
    alturaAtual -= 15,
    pdf_add_line(relatorio_pdf_doc, pagina, 60, alturaAtual, 500, alturaAtual, 2, PDF_BLACK);
    alturaAtual -= 30;
    pdf_add_text_wrap(relatorio_pdf_doc, pagina, nome_documento, 14, 60, alturaAtual, PDF_BLACK, 300, PDF_ALIGN_JUSTIFY);
    alturaAtual -= 30;
    pdf_set_font(relatorio_pdf_doc, "Helvetica-Bold");
    pdf_add_text_wrap(relatorio_pdf_doc, pagina, quantidade_vendas_documento, 16, 60, alturaAtual, PDF_BLACK, 300, PDF_ALIGN_JUSTIFY);
    pdf_set_font(relatorio_pdf_doc, "Helvetica");

    int i, j;
    char venda_titulo_documento[10];
    char venda_cliente_documento[110];
    char venda_produto_documento[175];
    char venda_produto_quantidade_documento[26];
    char venda_produto_preco_unitario_documento[30];
    char venda_produto_preco_documento[55];

    for (i = 0; i < encontrados; i++){
        if (alturaAtual - 200 <= 30){
            pagina = pdf_append_page(relatorio_pdf_doc);
            alturaAtual = 770;
        }

        sprintf(venda_titulo_documento, "Venda %d:", i + 1);
        sprintf(venda_cliente_documento, "Cliente: %s", relatorio[i].cliente.nome);
        sprintf(venda_produto_quantidade_documento, "%d produtos vendidos: ", relatorio[i].carrinho.tamanho);
        sprintf(venda_produto_preco_documento, "Preço Total: R$%.2lf - Desconto: %d%% ou R$%.2lf", relatorio[i].carrinho.preco_final, relatorio[i].carrinho.descontopercento, relatorio[i].carrinho.desconto);

        alturaAtual -= 20;
        pdf_add_text_wrap(relatorio_pdf_doc, pagina, venda_titulo_documento, 14, 60, alturaAtual, PDF_BLACK, 300, PDF_ALIGN_JUSTIFY);
        alturaAtual -= 20;
        pdf_add_text_wrap(relatorio_pdf_doc, pagina, venda_cliente_documento, 14, 60, alturaAtual, PDF_BLACK, 300, PDF_ALIGN_JUSTIFY);
        alturaAtual -= 20;
        pdf_add_text_wrap(relatorio_pdf_doc, pagina, venda_produto_quantidade_documento, 12, 60, alturaAtual, PDF_BLACK, 300, PDF_ALIGN_JUSTIFY);

        alturaAtual -= 15;
        pdf_add_line(relatorio_pdf_doc, pagina, 75, alturaAtual, 500, alturaAtual, 2, PDF_BLACK);
        alturaAtual -= 20;
        for (j = 0; j < relatorio[i].carrinho.tamanho; j++){
            if (alturaAtual - 70 <= 30){
                pagina = pdf_append_page(relatorio_pdf_doc);
                alturaAtual = 770;
            }
            
            sprintf(venda_produto_documento, "Produto: %s\nQuantidade: %d", relatorio[i].carrinho.produtos[j].nome, relatorio[i].carrinho.produtos[j].quantidade);
            sprintf(venda_produto_preco_unitario_documento, "Preço unitário: %.2lf", relatorio[i].carrinho.produtos[j].preco);

            pdf_add_text_wrap(relatorio_pdf_doc, pagina, venda_produto_documento, 14, 75, alturaAtual, PDF_BLACK, 300, PDF_ALIGN_JUSTIFY);
            alturaAtual -= 35;
            pdf_add_text_wrap(relatorio_pdf_doc, pagina, venda_produto_preco_unitario_documento, 12, 75, alturaAtual, PDF_BLACK, 300, PDF_ALIGN_JUSTIFY);
            alturaAtual -= 35;
        }

        if (alturaAtual - 35 <= 30){
            pagina = pdf_append_page(relatorio_pdf_doc);
            alturaAtual = 770;
        }
        
        alturaAtual += 20;
        pdf_add_line(relatorio_pdf_doc, pagina, 75, alturaAtual, 500, alturaAtual, 2, PDF_BLACK);
        alturaAtual -= 20;
        pdf_add_text_wrap(relatorio_pdf_doc, pagina, venda_produto_preco_documento, 12, 75, alturaAtual, PDF_BLACK, 300, PDF_ALIGN_JUSTIFY);
        alturaAtual -= 15;
        pdf_add_line(relatorio_pdf_doc, pagina, 60, alturaAtual, 500, alturaAtual, 2, PDF_BLACK);
    }

    pdf_save(relatorio_pdf_doc, "Relatorio.pdf");
    pdf_destroy(relatorio_pdf_doc);
    gtk_widget_destroy(dialog);

#ifdef _WIN32
    system("Relatorio.pdf");
#endif
}

void vendas_botao(GtkBuilder *interface){
    GtkWidget *botao_nova_venda, *botao_carrinho, *botao_remover_venda, *botao_relatorio;

    botao_nova_venda = (GtkWidget *) gtk_builder_get_object(interface, "botao_nova_venda");
    gtk_tool_item_set_tooltip_text((GtkToolItem *) botao_nova_venda, "Nova Venda");
    g_signal_connect(botao_nova_venda, "clicked", G_CALLBACK(nova_venda_dialog), interface);

    botao_carrinho = (GtkWidget *) gtk_builder_get_object(interface, "botao_carrinho");
    gtk_tool_item_set_tooltip_text((GtkToolItem *) botao_carrinho, "Carrinho");
    g_signal_connect(botao_carrinho, "clicked", G_CALLBACK(carrinho_dialog), interface);

    botao_remover_venda = (GtkWidget *) gtk_builder_get_object(interface, "botao_remover_venda");
    gtk_tool_item_set_tooltip_text((GtkToolItem *) botao_remover_venda, "Remover Venda");
    g_signal_connect(botao_remover_venda, "clicked", G_CALLBACK(remover_venda_dialog), interface);

    botao_relatorio = (GtkWidget *) gtk_builder_get_object(interface, "botao_relatorio");
    gtk_tool_item_set_tooltip_text((GtkToolItem *) botao_relatorio, "Gerar Relatório");
    g_signal_connect(botao_relatorio, "clicked", G_CALLBACK(relatorio_dialog), interface);
}

void vendas_cliente_completion(GtkWidget *inputCliente){
    GtkTreeModel *lista;
    GtkTreeIter lista_estrutura_primaria;

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

void vendas_produto_completion(GtkWidget *inputProduto){
    GtkTreeModel *lista;
    GtkTreeIter lista_estrutura_primaria;

    lista = (GtkTreeModel *) gtk_list_store_new(1, G_TYPE_STRING);

    int i;
    for (i = 0; i < PRODUTOS_LISTA->tamanho; i++){
        gtk_list_store_append((GtkListStore *) lista, &lista_estrutura_primaria);
        gtk_list_store_set((GtkListStore *) lista, &lista_estrutura_primaria, 0, PRODUTOS_LISTA->produtos[i].nome, -1);
    }

    GtkEntryCompletion *inputProdutoCompletion = gtk_entry_completion_new();
    gtk_entry_completion_set_model(inputProdutoCompletion, lista);
    gtk_entry_completion_set_text_column(inputProdutoCompletion, 0);
    gtk_entry_completion_set_minimum_key_length(inputProdutoCompletion, 1);
    gtk_entry_completion_set_popup_completion(inputProdutoCompletion, TRUE);

    gtk_entry_set_completion((GtkEntry *) inputProduto, inputProdutoCompletion);
}