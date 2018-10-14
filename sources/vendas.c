//  ;=====================================
//  ;  Title: vendas.c
//  ;  Author: Vinicius Costa Castro
//  ;  Date: 10/13/18
//  ;=====================================

#include "../headers/vendas.h"

Vendas *criar_vendas(){
    Vendas *nova_lista = (Vendas *) malloc(sizeof(Vendas));
    if (nova_lista == NULL)
        return NULL;
    nova_lista->tamanho = 0;
    nova_lista->vendas = NULL;
    return nova_lista;
}

void apagar_vendas(Vendas *nova_lista){
    free(nova_lista->vendas);
    free(nova_lista);
}

void nova_venda(Vendas *lista, struct venda venda){
    lista->vendas = (struct venda *) realloc(lista->vendas, (lista->tamanho + 1) * sizeof(struct venda));
    lista->vendas[lista->tamanho] = venda;
    lista->vendas[lista->tamanho].id = lista->tamanho;
    lista->vendas[lista->tamanho].horario = time(NULL);
    lista->tamanho++;
}

static int BuscaBinaria(Vendas *lista, int termo, int inicio, int fim){
    int meio = (inicio + fim) / 2;

    if (lista->vendas[meio].id == termo)
        return meio;
    if (inicio > fim)
        return -1;
    else
        if (lista->vendas[meio].id > termo)
            return BuscaBinaria(lista, termo, inicio, meio - 1);
        else
            return BuscaBinaria(lista, termo, meio + 1, fim);
}

struct cliente *buscar_venda(Vendas *lista, int venda){
    int pos = BuscaBinaria(lista, venda, 0, lista->tamanho - 1);
    if (pos == -1)
        return NULL;
    return &(lista->vendas[pos]);
}