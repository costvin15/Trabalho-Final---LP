#include <gtk/gtk.h>
#include <stdlib.h>

#include "clientes.c"
#include "headers/clientes.h"
#include "headers/produtos.h"

void salvar_ao_sair(GtkWidget *, Clientes *);

int main(int argc, char **argv){
    gtk_init(&argc, &argv);

    Clientes *clientes;
    GtkBuilder *interface;
    const gchar interface_nome[] = "interfaces/main.xml";
    GtkWidget *janela;

    clientes = reutilizar_clientes();
    interface = gtk_builder_new();
    if (!gtk_builder_add_from_file(interface, interface_nome, NULL)){
        fprintf(stderr, "Nao foi possivel inicializar a GUI.\n");
        exit(1);
    }

    janela = (GtkWidget *) gtk_builder_get_object(interface, "window");
    g_signal_connect(janela, "destroy", G_CALLBACK(salvar_ao_sair), clientes);
    g_signal_connect(janela, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    clientes_gui(interface, clientes);

    gtk_main();
}

void salvar_ao_sair(GtkWidget *janela, Clientes *clientes){
    salvar_clientes(clientes);
    apagar_clientes(clientes);
}