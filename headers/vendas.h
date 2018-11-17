#ifndef VENDAS_H
#define VENDAS_H

#include "clientes.h"
#include "carrinho.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct comprador {
    char nome[100];
    char endereco[210];
    char telefone[15];
};

struct venda {
    unsigned int index;
    time_t horario;
    struct comprador cliente;
    Carrinho carrinho;
};

typedef struct {
    int tamanho;
    struct venda *vendas;
} Vendas;

Vendas *reutilizar_vendas();
Vendas *criar_vendas();
void apagar_vendas(Vendas *);

void adicionar_venda(Vendas *, struct comprador, Carrinho *, time_t);

void popular_vendas(Vendas *lista);
void salvar_vendas(Vendas *);

void nova_venda(Vendas *, struct cliente, Carrinho *);
void remover_venda(Vendas *, int);

#endif