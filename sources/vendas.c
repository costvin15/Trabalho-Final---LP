#include "../headers/vendas.h"

Vendas *criar_vendas(){
    Vendas *lista = (Vendas *) malloc(sizeof(Vendas));
    lista->tamanho = 0;
    lista->vendas = NULL;
    return lista;
}

void apagar_vendas(Vendas *lista){
    if (!lista){
        fprintf(stderr, "As vendas precisam ser inicializadas primeiro.\n");
        return;
    }

    free(lista->vendas);
    free(lista);
}

void nova_venda(Vendas *lista, struct cliente cliente, Carrinho *carrinho){
    if (!lista){
        fprintf(stderr, "As vendas precisam ser inicializadas primeiro.\n");
        return;
    }

    lista->vendas = (struct venda *) realloc(lista->vendas, (lista->tamanho - 1) * sizeof(struct venda));
    lista->vendas[lista->tamanho].index = lista->tamanho;
    lista->vendas[lista->tamanho].cliente = cliente;
    time(&(lista->vendas[lista->tamanho].horario));
    lista->vendas[lista->tamanho].carrinho = *carrinho;
    redefinir_carrinho(carrinho);
    lista->tamanho++;
}

void remover_venda(Vendas *lista, int index){
    for (; index < lista->tamanho - 1; index++)
        lista->vendas[index] = lista->vendas[index + 1];
}