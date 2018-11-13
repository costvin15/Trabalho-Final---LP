#include <gtk/gtk.h>
#include <stdlib.h>

#include "clientes.c"
#include "produtos.c"
#include "vendas.c"

#include "headers/clientes.h"
#include "headers/produtos.h"
#include "headers/vendas.h"

void salvar_ao_sair(GtkWidget *, void **);

int main(int argc, char **argv){
    gtk_init(&argc, &argv);

    Clientes *clientes;
    Produtos *produtos;
    Vendas *vendas;

    GtkBuilder *interface;
    const gchar interface_nome[] = "interfaces/main.xml";
    GtkWidget *janela;

    clientes = reutilizar_clientes();
    produtos = reutilizar_produtos();
    vendas = reutilizar_vendas();

    interface = gtk_builder_new();
    if (!gtk_builder_add_from_file(interface, interface_nome, NULL)){
        fprintf(stderr, "Nao foi possivel inicializar a GUI.\n");
        exit(1);
    }

    janela = (GtkWidget *) gtk_builder_get_object(interface, "window");

    void **argumentos = (void **) malloc(2 * sizeof(void *));
    argumentos[0] = clientes;
    argumentos[1] = produtos;

    g_signal_connect(janela, "destroy", G_CALLBACK(salvar_ao_sair), argumentos);
    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    clientes_gui(interface, clientes);
    produtos_gui(interface, produtos);
    vendas_gui(interface, vendas);

    gtk_main();
}

void salvar_ao_sair(GtkWidget *janela, void **argumentos){
    Clientes *clientes = (Clientes *) argumentos[0];
    Produtos *produtos = (Produtos *) argumentos[1];

    salvar_clientes(clientes);
    apagar_clientes(clientes);

    salvar_produtos(produtos);
    apagar_produtos(produtos);
}