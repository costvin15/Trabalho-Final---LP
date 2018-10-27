#include "../headers/vendas.h"

Vendas *criar_vendas(){
    Vendas *nova_lista = (Vendas *) malloc(sizeof(Vendas));
    nova_lista->tamanho = 0;
    nova_lista->vendas = NULL;
    return nova_lista;
}

void apagar_vendas(Vendas *nova_lista){
    free(nova_lista->vendas);
    free(nova_lista);
}

int adicionar_venda(Vendas *lista, struct produto produto, struct cliente cliente){
    lista->vendas = (struct venda *) realloc(lista->vendas, (lista->tamanho + 1) * sizeof(struct venda));
    if (lista->vendas == NULL){
        fprintf(stderr, "Nao ha espaco disponivel.");
        return false;
    }

    lista->vendas[lista->tamanho].cliente = cliente;
    lista->vendas[lista->tamanho].produto = produto;
    lista->vendas[lista->tamanho].horario = time(NULL);
    lista->tamanho++;
    return true;
}