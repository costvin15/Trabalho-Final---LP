#ifndef VENDAS_H
#define VENDAS_H

#include "clientes.h"
#include "carrinho.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

struct venda {
    unsigned int index;
    time_t horario;
    struct cliente cliente;
    Carrinho carrinho;
};

typedef struct {
    int tamanho;
    struct venda *vendas;
} Vendas;

Vendas *criar_vendas();
void apagar_vendas(Vendas *);

void nova_venda(Vendas *, struct cliente, Carrinho *);
void remover_venda(Vendas *, int);

#endif